#include "SpaceshipCharacter.h"

#include "Components/ArrowComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "ASoundSphere.h"
#include "EngineUtils.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
// Required Includes for new features
#include "NiagaraComponentPoolMethodEnum.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/PlayerController.h"
#include "Camera/CameraShakeBase.h"
#include "Particles/ParticleSystemComponent.h"
#include "FVFXOption.h"
#include "Sound/SoundBase.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "TimerManager.h"
#include "UWBP_HUD_Base.h"
#include "GameFramework/ProjectileMovementComponent.h"


ASpaceshipCharacter::ASpaceshipCharacter()
{
    PrimaryActorTick.bCanEverTick = true;
    ThrustersSpeedParam = FName(TEXT("Thrusters_Speed"));
    ParticulateSpeedParam = FName(TEXT("Particulate_Speed"));
    EnergyCoreSpeedParam = FName(TEXT("EnergyCore_Speed"));
    HeatHazeSpeedParam = FName(TEXT("HeatHaze_Speed"));

    USkeletalMeshComponent* MeshRef = GetMesh();
    if (!MeshRef) return;

    // --- Camera Setup ---
    // Create Spring Arm (Camera Boom)
    SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
    SpringArmComp->SetupAttachment(MeshRef); // Attach to mesh or RootComponent as needed
    SpringArmComp->bUsePawnControlRotation = true; // Rotate arm based on controller
    SpringArmComp->bInheritPitch = true;
    SpringArmComp->bInheritYaw = true;
    SpringArmComp->bInheritRoll = false; // Keep the camera upright relative to the boom

    // Create Camera attached to Spring Arm
    CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
    CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName); // Attach to end of boom
    CameraComp->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

    // --- Projectile Spawn Points ---
    ProjectileSpawnPointWhite = CreateDefaultSubobject<UArrowComponent>(TEXT("ProjectileSpawnWhite"));
    ProjectileSpawnPointWhite->SetupAttachment(MeshRef);

    ProjectileSpawnPointBlack = CreateDefaultSubobject<UArrowComponent>(TEXT("ProjectileSpawnBlack"));
    ProjectileSpawnPointBlack->SetupAttachment(MeshRef);

    // --- Character Movement Configuration ---
    GetCharacterMovement()->bOrientRotationToMovement = false; // Character rotation is controlled by input/camera
    GetCharacterMovement()->bUseControllerDesiredRotation = true; // Character rotates towards control rotation
    GetCharacterMovement()->RotationRate = FRotator(0.f, 360.f, 0.f); // Adjust rotation speed as needed
    GetCharacterMovement()->DefaultLandMovementMode = MOVE_Flying;
    GetCharacterMovement()->MaxFlySpeed = 2400.0f;
    GetCharacterMovement()->GravityScale = 0.0f; 
    GetCharacterMovement()->MaxAcceleration = 4000.0f; // Exemple : Augmenter l'accélération
    GetCharacterMovement()->BrakingDecelerationFlying = 1000.0f; // Exemple : Décélération modérée

    //Thrusters
    LeftThrusterVFX = CreateDefaultSubobject<UNiagaraComponent>(TEXT("LeftThrusterVFX"));
    LeftThrusterVFX->SetupAttachment(GetMesh()); // Ou attachez à un socket spécifique si nécessaire
    LeftThrusterVFX->bAutoActivate = false;

    RightThrusterVFX = CreateDefaultSubobject<UNiagaraComponent>(TEXT("RightThrusterVFX"));
    RightThrusterVFX->SetupAttachment(GetMesh()); // Ou attachez à un socket spécifique si nécessaire
    RightThrusterVFX->bAutoActivate = false;

  
    // --- Default Values ---
    bUseControllerRotationYaw = false; // Let the movement component handle yaw rotation based on controller
    bUseControllerRotationPitch = false;
    bUseControllerRotationRoll = false;
    bCanToggleColor = true;

    HitMID = nullptr; // Initialize dynamic material instance
}

void ASpaceshipCharacter::BeginPlay()
{
    Super::BeginPlay();
    CurrentHealth = MaxHealth;
    
    // Update initial health display
    if (HUDWidgetClass)
    {
        UE_LOG(LogTemp, Log, TEXT("Creating HUD Widget"));
        APlayerController* PC = Cast<APlayerController>(GetController());
        if (PC)
        {
            HUDWidget = CreateWidget<UWBP_HUD_Base>(PC, HUDWidgetClass);
            if (HUDWidget)
            {
                HUDWidget->AddToViewport();
                HUDWidget->UpdateHealth(CurrentHealth / MaxHealth);
                UE_LOG(LogTemp, Log, TEXT("HUD Widget created and added to viewport"));
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("Failed to create HUD Widget"));
            }
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to get PlayerController"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("HUDWidgetClass is not set"));
    }
    // --- Update Camera Boom Settings ---
    if (SpringArmComp)
    {
        SpringArmComp->TargetArmLength = CameraBoomLength;
        SpringArmComp->bEnableCameraLag = bEnableCameraLag;
        SpringArmComp->CameraLagSpeed = CameraLagSpeed;
        SpringArmComp->bEnableCameraRotationLag = bEnableCameraRotationLag;
        SpringArmComp->CameraRotationLagSpeed = CameraRotationLagSpeed;
    }

    SaveManager = USpaceshipSaveManager::GetSaveManager(GetWorld());

    for (TActorIterator<AObjectPoolManager> It(GetWorld()); It; ++It)
    {
        PoolManager = *It;
        break;
    }

    UpdateMaterials();
    if (ThrusterEffect)
    {
        LeftThrusterVFX->SetAsset(ThrusterEffect);
        RightThrusterVFX->SetAsset(ThrusterEffect);
    }
    // --- Setup Hit Post Process Material ---
    if (HitPostProcessMaterial)
    {
        HitMID = UMaterialInstanceDynamic::Create(HitPostProcessMaterial, this);
        if (CameraComp) // Apply to our camera initially (but don't enable blend yet)
        {
             // Ensure the MID is added but weight is 0 initially.
             // We'll control blend weight in HandleHit.
            CameraComp->PostProcessSettings.AddBlendable(HitMID, 0.0f);
        }
    }
}


void ASpaceshipCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (bIsInSpaceship)
    {
        HandleSpaceshipMovement(DeltaTime);
        UpdateThrusterParametersVisuals(DeltaTime); // <- APPELER LA MISE A JOUR DES VFX ICI
    }
}

void ASpaceshipCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    APlayerController* PC = Cast<APlayerController>(GetController());
    if (PC)
    {
        UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer());
        if (Subsystem)
        {
            Subsystem->AddMappingContext(DefaultMappingContext, 0);
        }
    }

    UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);
    if (EnhancedInputComponent)
    {
        EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ASpaceshipCharacter::Move);
        EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Triggered, this, &ASpaceshipCharacter::HandleFire);
        EnhancedInputComponent->BindAction(SwitchModeAction, ETriggerEvent::Triggered, this, &ASpaceshipCharacter::HandleSwitchMode);
        EnhancedInputComponent->BindAction(ExitShipAction, ETriggerEvent::Triggered, this, &ASpaceshipCharacter::HandleExitShip);
    }
}
void ASpaceshipCharacter::Move(const FInputActionValue& Value)
{
    if (!bIsInSpaceship) return;

    // 1. Store the raw input vector
    CurrentMovementInput = Value.Get<FVector2D>(); // <- STORE INPUT

    // --- Reste de la logique de mouvement (AddMovementInput, UpdateRotation) ---
    // NE PLUS appeler UpdateThrusterParameters ici
    if (!CurrentMovementInput.IsNearlyZero())
    {
        const FVector ForwardDirection = GetActorForwardVector();
        const FVector RightDirection = GetActorRightVector();
        const FVector ForwardDirectionXY = FVector(ForwardDirection.X, ForwardDirection.Y, 0.0f).GetSafeNormal();
        const FVector RightDirectionXY = FVector(RightDirection.X, RightDirection.Y, 0.0f).GetSafeNormal();
        FVector DesiredMovementDirection =
            (ForwardDirectionXY * CurrentMovementInput.Y) + (RightDirectionXY * CurrentMovementInput.X);

        if (!DesiredMovementDirection.IsNearlyZero())
        {
            DesiredMovementDirection.Normalize();
            AddMovementInput(DesiredMovementDirection, 1.0f);
        }
        UpdateRotation(CurrentMovementInput.X, -CurrentMovementInput.Y);
    }
    else
    {
        UpdateRotation(0.0f, 0.0f);
        CurrentMovementInput = FVector2D::ZeroVector; // Reset stored input if no input
    }
}

void ASpaceshipCharacter::HandleFire(const FInputActionValue& Value)
{
    FireProjectile();
}

void ASpaceshipCharacter::HandleSwitchMode(const FInputActionValue& Value)
{
    // 1. Vérifier si le changement est autorisé (pas en cooldown)
    if (!bCanToggleColor)
    {
        // Si on est en cooldown, ne rien faire
        return;
    }

    // 2. Si autorisé, exécuter le changement de couleur
    ToggleColor(); // Appelle votre fonction existante

    // 3. Démarrer le cooldown
    bCanToggleColor = false; // Interdire le changement immédiatement
    // Démarrer un timer qui appellera ResetColorToggleCooldown après la durée spécifiée
    GetWorldTimerManager().SetTimer(
        ColorToggleCooldownTimerHandle,    // Le handle à utiliser
        this,                              // L'objet sur lequel appeler la fonction
        &ASpaceshipCharacter::ResetColorToggleCooldown, // La fonction à appeler
        ColorToggleCooldownDuration,       // Le délai avant d'appeler la fonction
        false                              // Ne pas faire répéter le timer (false)
    );
}


void ASpaceshipCharacter::ResetColorToggleCooldown()
{
    bCanToggleColor = true; // Réautoriser le changement de couleur

    // Optionnel : Vous pouvez "nettoyer" le handle si vous voulez, mais ce n'est pas
    // obligatoire pour un timer non répétitif qui a déjà expiré.
    // GetWorldTimerManager().ClearTimer(ColorToggleCooldownTimerHandle);
}


void ASpaceshipCharacter::HandleExitShip(const FInputActionValue& Value)
{
    ToggleSpaceshipMode();
}
void ASpaceshipCharacter::UpdateRotation(float RollInput, float PitchInput)
{
    const float DeltaTime = GetWorld()->GetDeltaSeconds();
    const float InterpSpeed = 5.0f;  // Vitesse de transition pour les rotations

    // Calcul des angles cibles
    float TargetRoll = RollInput * MaxRollAngle;
    float TargetPitch = PitchInput * MaxPitchAngle;

    // Obtenir la rotation actuelle
    FRotator NewRotation = GetActorRotation();

    // Interpoler doucement vers les angles cibles
    NewRotation.Roll = FMath::FInterpTo(NewRotation.Roll, TargetRoll, DeltaTime, InterpSpeed);
    NewRotation.Pitch = FMath::FInterpTo(NewRotation.Pitch, TargetPitch, DeltaTime, InterpSpeed);

    // Appliquer la nouvelle rotation
    SetActorRotation(NewRotation);
}

void ASpaceshipCharacter::HandleSpaceshipMovement(float DeltaTime)
{
    // Add damping/drag if needed
    UCharacterMovementComponent* MoveComp = GetCharacterMovement();
    if (MoveComp && MoveComp->IsFlying()) // Only apply drag while flying
    {
        // Dans HandleSpaceshipMovement
        FVector CurrentVelocity = MoveComp->Velocity;
        float SpeedSquared = CurrentVelocity.SizeSquared();
        float DragCoefficient = 0.001f; // Ajuster cette valeur !
        FVector DragForce = -CurrentVelocity.GetSafeNormal() * SpeedSquared * DragCoefficient;
        MoveComp->AddForce(DragForce);
    }
}


void ASpaceshipCharacter::FireProjectile()
{
    // Check if the Projectile Class is set
    if (!ProjectileClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("ProjectileClass is not set in SpaceshipCharacter!"));
        return;
    }

    // Ensure both spawn points are valid
    if (!ProjectileSpawnPointWhite || !ProjectileSpawnPointBlack) return;

    UWorld* const World = GetWorld();
    if (World)
    {
        const FNiagaraEffectPair SelectedEffects = GetSelectedVFXPair();
        FActorSpawnParameters SpawnParams;
        SpawnParams.Owner = this;
        SpawnParams.Instigator = GetInstigator();
        SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

        // Spawn projectile from the white spawn point
        FVector SpawnLocationWhite = ProjectileSpawnPointWhite->GetComponentLocation();
        FRotator SpawnRotationWhite = FRotator::ZeroRotator;

        ANiagaraProjectile* ProjectileWhite = World->SpawnActor<ANiagaraProjectile>(
            ProjectileClass,
            SpawnLocationWhite,
            SpawnRotationWhite,
            SpawnParams
        );

        if (ProjectileWhite)
        {
            ProjectileWhite->SetNiagaraEffects(SelectedEffects); // <-- Appel de la nouvelle fonction

            const FVector FixedWorldDirection = FVector(1.0f, 0.0f, 0.0f); // +X world
            if (UProjectileMovementComponent* MoveComp = ProjectileWhite->ProjectileMovement)
            {
                MoveComp->Velocity = FixedWorldDirection * MoveComp->InitialSpeed;
                MoveComp->InitialSpeed = ProjectileWhite->InitialSpeed;
                MoveComp->MaxSpeed = ProjectileWhite->MaxSpeed;
            }
        }

        // Spawn projectile from the black spawn point
        FVector SpawnLocationBlack = ProjectileSpawnPointBlack->GetComponentLocation();
        FRotator SpawnRotationBlack = FRotator::ZeroRotator;

        ANiagaraProjectile* ProjectileBlack = World->SpawnActor<ANiagaraProjectile>(
            ProjectileClass,
            SpawnLocationBlack,
            SpawnRotationBlack,
            SpawnParams
        );

        if (ProjectileBlack)
        {
            ProjectileBlack->SetNiagaraEffects(SelectedEffects); // <-- Appel de la nouvelle fonction

            const FVector FixedWorldDirection = FVector(1.0f, 0.0f, 0.0f); // +X world
            if (UProjectileMovementComponent* MoveComp = ProjectileBlack->ProjectileMovement)
            {
                MoveComp->Velocity = FixedWorldDirection * MoveComp->InitialSpeed;
                MoveComp->InitialSpeed = ProjectileBlack->InitialSpeed;
                MoveComp->MaxSpeed = ProjectileBlack->MaxSpeed;
            }
        }

        // Start Fire Cooldown
        bCanFire = false;
        World->GetTimerManager().SetTimer(FireCooldownTimer, [this]() { bCanFire = true; }, FireCooldown, false);
    }
}
void ASpaceshipCharacter::ToggleColor()
{
    bIsWhiteMode = !bIsWhiteMode;
    UpdateMaterials();
}

void ASpaceshipCharacter::ToggleSpaceshipMode()
{
    bIsInSpaceship = !bIsInSpaceship;

    UCharacterMovementComponent* Movement = GetCharacterMovement();
    if (Movement)
    {
        Movement->SetMovementMode(bIsInSpaceship ? MOVE_Flying : MOVE_Walking);
        // Reset velocity when switching modes?
        // Movement->Velocity = FVector::ZeroVector;
    }

    // Potentially enable/disable camera boom lag or change length
    if (SpringArmComp)
    {
        // Example: Disable lag when not in spaceship
        // SpringArmComp->bEnableCameraLag = bIsInSpaceship && bEnableCameraLag;
    }
}

float ASpaceshipCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, 
                                     AController* EventInstigator, AActor* DamageCauser)
{
    // Vérification des couleurs (code existant)
    ASoundSphere* Projectile = Cast<ASoundSphere>(DamageCauser);
    if (Projectile && Projectile->GetColor() == bIsWhiteMode)
    {
        return 0.0f;
    }

    // Appliquer les dégâts
    float ActualDamage = DamagePerHit;  // Utiliser une valeur fixe
    CurrentHealth = FMath::Max(0.0f, CurrentHealth - ActualDamage);

    // Arrêter la régénération et relancer le délai
    StopHealthRegeneration();
    GetWorld()->GetTimerManager().SetTimer(
        HealthRegenDelayHandle,
        this,
        &ASpaceshipCharacter::StartHealthRegeneration,
        HealthRegenDelay,
        false
    );

    if (HUDWidget)
    {
        HUDWidget->UpdateHealth(CurrentHealth / MaxHealth);
    }
    
    if (ActualDamage > 0.0f)
    {
        HandleHit(ActualDamage, DamageEvent, DamageCauser);
    }

    return ActualDamage;
}

void ASpaceshipCharacter::HandleHit(float DamageAmount, FDamageEvent const& DamageEvent, AActor* DamageCauser)
{
    APlayerController* PC = Cast<APlayerController>(GetController());
    if (!PC) return;

    // 1. Camera Shake
    if (HitCameraShakeClass)
    {
        // Scale shake based on damage (optional, simple linear scale example)
        float ShakeScale = FMath::GetMappedRangeValueClamped(FVector2D(1.0f, 50.0f), FVector2D(0.5f, 2.0f), DamageAmount);
        PC->ClientStartCameraShake(HitCameraShakeClass, ShakeScale);
    }

    // 2. Post-Process Effect
    if (HitMID) // Check if dynamic material instance exists
    {
        // Scale intensity based on damage (optional)
        float EffectIntensity = FMath::GetMappedRangeValueClamped(FVector2D(1.0f, 50.0f), FVector2D(0.5f, 1.0f), DamageAmount);
        TriggerHitPostProcess(EffectIntensity);
    }

    // 3. Particle Effect
    if (HitParticleEffect)
    {
        // Spawn attached to the mesh or at a specific location
        UGameplayStatics::SpawnEmitterAttached(
            HitParticleEffect,
            GetMesh(), // Attach to the skeletal mesh
            NAME_None, // Optional socket name
            GetActorLocation(), // Use actor location as fallback/offset base
            GetActorRotation(),
            EAttachLocation::KeepWorldPosition, // Or SnapToTarget
            true // Auto destroy
        );
        // Alternatively, spawn at the impact point if available from DamageEvent
        // FVector ImpactLocation = GetActorLocation(); // Default
        // FPointDamageEvent* PointDamageEvent = (FPointDamageEvent*)(&DamageEvent);
        // if (PointDamageEvent) { ImpactLocation = PointDamageEvent->HitInfo.ImpactPoint; }
        // UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitParticleEffect, ImpactLocation, GetActorRotation());
    }

    // 4. Sound Effect
    if (HitSound)
    {
        UGameplayStatics::PlaySoundAtLocation(this, HitSound, GetActorLocation());
    }
}

void ASpaceshipCharacter::TriggerHitPostProcess(float Intensity)
{
    if (HitMID && CameraComp)
    {
        // Set the intensity parameter (if the parameter name is valid)
        HitMID->SetScalarParameterValue(HitEffectIntensityParamName, Intensity);

        // Set blend weight to 1 to activate the effect
        CameraComp->PostProcessSettings.AddBlendable(HitMID, Intensity);


        // Set a timer to clear the effect
        GetWorldTimerManager().SetTimer(HitEffectTimerHandle, this, &ASpaceshipCharacter::ClearHitPostProcess, HitEffectDuration, false);
    }
}

void ASpaceshipCharacter::ClearHitPostProcess()
{
     if (HitMID && CameraComp)
    {
         // Reset blend weight to 0 to deactivate
         CameraComp->PostProcessSettings.RemoveBlendable(HitMID);
    }
     GetWorldTimerManager().ClearTimer(HitEffectTimerHandle);
}


void ASpaceshipCharacter::UpdateMaterials()
{
    if (!SaveManager || !SaveManager->CurrentSave || !SkinOptions) return;

    int32 SkinIndex = SaveManager->CurrentSave->ColorSkinID;
    if (!SkinOptions->AvailableSkins.IsValidIndex(SkinIndex)) return;

    const FSkinOption& CurrentSkin = SkinOptions->AvailableSkins[SkinIndex];
    const FMaterialSet& Materials = CurrentSkin.GameplayMaterialSets;

    USkeletalMeshComponent* MeshRef = GetMesh();
    if (!MeshRef) return;

    UMaterialInterface* TargetMaterial = bIsWhiteMode ? Materials.WhiteMaterial : Materials.BlackMaterial;
    if (TargetMaterial)
    {
        // Assuming the main material is at index 0, adjust if needed
        MeshRef->SetMaterial(0, TargetMaterial);
    }

    // Update any other material slots if necessary
    // for (int32 i = 0; i < MeshRef->GetNumMaterials(); i++)
    // {
    //     MeshRef->SetMaterial(i, TargetMaterial);
    // }
}


void ASpaceshipCharacter::Enter(ACharacter* Character)
{
    if (!Character) return;

    PlayerCharacter = Character;
    AController* ControllerRef = Character->GetController();
    if (ControllerRef)
    {
        ControllerRef->Possess(this);
    }

    Character->SetActorHiddenInGame(true);
    Character->SetActorEnableCollision(false);
    Character->SetActorTickEnabled(false);

    // Enable tick for spaceship
     SetActorTickEnabled(true);
     bIsInSpaceship = true;
}

void ASpaceshipCharacter::Exit()
{
    if (!PlayerCharacter) return;

    AController* ControllerRef = GetController();
    if (ControllerRef)
    {
        ControllerRef->Possess(PlayerCharacter);
    }

    PlayerCharacter->SetActorHiddenInGame(false);
    PlayerCharacter->SetActorEnableCollision(true);
    PlayerCharacter->SetActorTickEnabled(true);

    // Disable tick for spaceship
    SetActorTickEnabled(false);
    bIsInSpaceship = false;
    PlayerCharacter = nullptr;
}
void ASpaceshipCharacter::UpdateThrusterParametersVisuals(float DeltaTime)
{
    if (!LeftThrusterVFX || !RightThrusterVFX) return;

    FVector CurrentVelocity = GetVelocity();
    float CurrentSpeed = CurrentVelocity.Size();

    UCharacterMovementComponent* MoveComp = GetCharacterMovement();
    float MaxSpeed = MoveComp ? MoveComp->MaxFlySpeed : 2400.0f;
    float SpeedRatio = (MaxSpeed > 0) ? FMath::Clamp(CurrentSpeed / MaxSpeed, 0.0f, 1.0f) : 0.0f;

    // --- Logique d'Activation/Désactivation ---
    // Exemple: Activer si vitesse > 50 ou si input avant/arrière est donné
    const float ActivationSpeedThreshold = 50.0f;
    bool bShouldBeActive = (CurrentSpeed > ActivationSpeedThreshold) || !FMath::IsNearlyZero(CurrentMovementInput.Y);

    if (!bShouldBeActive)
    {
        if (LeftThrusterVFX->IsActive()) LeftThrusterVFX->Deactivate();
        if (RightThrusterVFX->IsActive()) RightThrusterVFX->Deactivate();
        return; // Sortir si les thrusters doivent être éteints
    }
    else
    {
        if (!LeftThrusterVFX->IsActive()) LeftThrusterVFX->Activate(true);
        if (!RightThrusterVFX->IsActive()) RightThrusterVFX->Activate(true);
    }

    // Calcul de l'intensité de base en fonction de la vitesse
    float BaseIntensity = SpeedRatio;
    BaseIntensity = FMath::Clamp(BaseIntensity, 0.0f, 1.0f); // Garder entre 0 et 1

    // --- NOUVELLE LOGIQUE: Ajustement pour mouvement avant/arrière ---
    // Nous utilisons l'input Y pour ajuster l'intensité des propulseurs
    // Input Y positif = avant, donc augmenter l'intensité des propulseurs
    // Input Y négatif = arrière, donc diminuer l'intensité des propulseurs
    float ForwardInput = CurrentMovementInput.Y;
    
    // Coefficient d'ajustement pour l'input avant/arrière (à régler selon préférence)
    const float ForwardAdjustFactor = 0.7f;
    
    // Appliquer un ajustement à l'intensité de base en fonction de l'input avant/arrière
    if (!FMath::IsNearlyZero(ForwardInput))
    {
        // Si on avance (Input Y > 0), augmenter l'intensité
        if (ForwardInput > 0)
        {
            // Plus on pousse vers l'avant, plus les propulseurs sont intenses
            BaseIntensity *= (1.0f + (ForwardInput * ForwardAdjustFactor));
        }
        // Si on recule (Input Y < 0), diminuer l'intensité
        else
        {
            // Plus on recule, moins les propulseurs sont intenses
            BaseIntensity *= (1.0f - (FMath::Abs(ForwardInput) * ForwardAdjustFactor));
        }
    }

    const float VISUAL_MULTIPLIER = 400.0f; // Ajustez cette valeur si nécessaire
    float VisualIntensity = BaseIntensity * VISUAL_MULTIPLIER;

    // --- Différenciation Gauche/Droite ---
    FVector LocalVelocity = GetActorTransform().InverseTransformVectorNoScale(CurrentVelocity);
    float SidewaysInput = CurrentMovementInput.X;

    float LeftThrusterIntensity = VisualIntensity;
    float RightThrusterIntensity = VisualIntensity;

    // Ajuster en fonction du mouvement/input latéral
    float TurnFactor = 0.5f; // Force de l'effet de différenciation
    if (!FMath::IsNearlyZero(SidewaysInput))
    {
        // Si on tourne/strafe à droite (Input X > 0)
        if (SidewaysInput > 0)
        {
            // Augmenter le gauche, diminuer le droit (pour tourner à droite visuellement)
            LeftThrusterIntensity *= (1.0f + TurnFactor);
            RightThrusterIntensity *= (1.0f - TurnFactor);
        }
        // Si on tourne/strafe à gauche (Input X < 0)
        else
        {
            // Augmenter le droit, diminuer le gauche
            RightThrusterIntensity *= (1.0f + TurnFactor);
            LeftThrusterIntensity *= (1.0f - TurnFactor);
        }
    }
    
    // Clamp pour éviter les valeurs négatives si TurnFactor est grand
    LeftThrusterIntensity = FMath::Max(0.0f, LeftThrusterIntensity);
    RightThrusterIntensity = FMath::Max(0.0f, RightThrusterIntensity);

    // --- Application aux Paramètres Niagara ---
    auto UpdateNiagaraParams = [&](UNiagaraComponent* Thruster, float Intensity)
    {
        // Appliquer les multiplicateurs spécifiques à chaque paramètre RELATIFS à l'intensité calculée
        Thruster->SetVariableFloat(ThrustersSpeedParam, Intensity * THRUSTER_FORCE_MULTIPLIER);
        Thruster->SetVariableFloat(ParticulateSpeedParam, Intensity * PARTICULATE_FORCE_MULTIPLIER);
        Thruster->SetVariableFloat(EnergyCoreSpeedParam, Intensity * ENERGY_CORE_FORCE_MULTIPLIER);
        Thruster->SetVariableFloat(HeatHazeSpeedParam, Intensity * HEAT_HAZE_FORCE_MULTIPLIER);
    };

    UpdateNiagaraParams(LeftThrusterVFX, LeftThrusterIntensity);
    UpdateNiagaraParams(RightThrusterVFX, RightThrusterIntensity);
}

FNiagaraEffectPair ASpaceshipCharacter::GetSelectedVFXPair() const
{
    // 1. Vérifier SaveManager et CurrentSave
    if (!SaveManager || !SaveManager->CurrentSave)
    {
        UE_LOG(LogTemp, Warning, TEXT("ASpaceshipCharacter::GetSelectedVFXPair - SaveManager ou CurrentSave invalide."));
        return FNiagaraEffectPair(); // Retourne une paire vide
    }

    // 2. Lire l'ID sauvegardé (en utilisant ProjectileSkinID comme déterminé dans l'analyse)
    const int32 SelectedVFXID = SaveManager->CurrentSave->ProjectileSkinID;

    // 3. Vérifier le Data Asset VFXOptions
    if (!VFXOptions)
    {
        UE_LOG(LogTemp, Error, TEXT("ASpaceshipCharacter::GetSelectedVFXPair - VFXOptions Data Asset n'est pas assigné !"));
        return FNiagaraEffectPair(); // Retourne une paire vide
    }

    // 4. Vérifier la validité de l'index dans le tableau AvailableVFX
    if (!VFXOptions->AvailableVFX.IsValidIndex(SelectedVFXID))
    {
        UE_LOG(LogTemp, Warning, TEXT("ASpaceshipCharacter::GetSelectedVFXPair - Index VFX sauvegardé (%d) invalide pour le Data Asset '%s'. Taille du tableau: %d"),
               SelectedVFXID, *VFXOptions->GetName(), VFXOptions->AvailableVFX.Num());

        // Fallback : Retourner la première paire si elle existe, sinon une paire vide
        if (VFXOptions->AvailableVFX.IsValidIndex(0))
        {
            return VFXOptions->AvailableVFX[0].Effects;
        }
        else
        {
            return FNiagaraEffectPair();
        }
    }

    // 5. Retourner la paire d'effets correspondante
    return VFXOptions->AvailableVFX[SelectedVFXID].Effects;
}

void ASpaceshipCharacter::StartHealthRegeneration()
{
    if (CurrentHealth >= MaxHealth) return;

    GetWorld()->GetTimerManager().SetTimer(
        HealthRegenTimerHandle,
        this,
        &ASpaceshipCharacter::RegenerateHealth,
        0.1f,  // Régénérer toutes les 0.1 secondes
        true   // Répéter
    );
}

void ASpaceshipCharacter::StopHealthRegeneration()
{
    GetWorld()->GetTimerManager().ClearTimer(HealthRegenTimerHandle);
    GetWorld()->GetTimerManager().ClearTimer(HealthRegenDelayHandle);
}
void ASpaceshipCharacter::RegenerateHealth()
{
    if (CurrentHealth >= MaxHealth)
    {
        StopHealthRegeneration();
        return;
    }

    float HealthToAdd = (HealthRegenRate * 0.1f);
    CurrentHealth = FMath::Min(MaxHealth, CurrentHealth + HealthToAdd);

    if (APlayerController* PC = Cast<APlayerController>(GetController()))
    {
        if (UWBP_HUD_Base* HUD = Cast<UWBP_HUD_Base>(PC->GetHUD()))
        {
            HUD->UpdateHealth(CurrentHealth / MaxHealth);
        }
    }
}

void ASpaceshipCharacter::CreateHUDWidget()
{
    if (HUDWidgetClass)
    {
        APlayerController* PC = Cast<APlayerController>(GetController());
        if (PC)
        {
            HUDWidget = CreateWidget<UWBP_HUD_Base>(PC, HUDWidgetClass);
            if (HUDWidget)
            {
                HUDWidget->AddToViewport();
                HUDWidget->UpdateHealth(CurrentHealth / MaxHealth);
            }
        }
    }
}