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
#include "Sound/SoundBase.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "TimerManager.h"
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

    // Initialisation de l'intensité
    ThrusterIntensity = 0.0f;
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
    }
    // Note: Camera rotation smoothness is largely handled by the SpringArmComponent settings now.
    // Additional manual smoothing could be added here if required.
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

// SpaceshipCharacter.cpp -> Move function

void ASpaceshipCharacter::Move(const FInputActionValue& Value)
{
    if (!bIsInSpaceship) return;

    // 1. Get the raw 2D input vector
    const FVector2D MovementVector = Value.Get<FVector2D>();
    
    // Utiliser la vitesse actuelle du Character pour une force plus dynamique
    float ThrusterForce = FMath::Abs(MovementVector.Y) * 400.0f;
    UpdateThrusterParameters(ThrusterForce);


    // 2. Get base directions projected onto XY plane
    const FVector ForwardDirection = GetActorForwardVector();
    const FVector RightDirection = GetActorRightVector();
    const FVector ForwardDirectionXY = FVector(ForwardDirection.X, ForwardDirection.Y, 0.0f).GetSafeNormal();
    const FVector RightDirectionXY = FVector(RightDirection.X, RightDirection.Y, 0.0f).GetSafeNormal();

    // 3. Calculate the combined desired world-space movement direction
    //    MovementVector.Y controls forward/backward (along ForwardDirectionXY)
    //    MovementVector.X controls right/left (along RightDirectionXY)
    FVector DesiredMovementDirection =
        (ForwardDirectionXY * MovementVector.Y) + (RightDirectionXY * MovementVector.X);

    // 4. Normalize the final direction vector (if non-zero) to ensure consistent speed regardless of angle
    //    The CharacterMovementComponent's MaxFlySpeed will handle the actual speed limit.
    if (!DesiredMovementDirection.IsNearlyZero())
    {
        DesiredMovementDirection.Normalize();

        // 5. Apply the combined movement input
        //    We pass a scale of 1.0f because the direction is normalized,
        //    and speed is controlled by CharacterMovementComponent properties (MaxFlySpeed, Acceleration).
        //    Note: We are NOT multiplying by ThrustForce here anymore, assuming speed is handled by movement component settings.
        //    If you want direct force application, you'd calculate the force magnitude based on input and apply it.
        AddMovementInput(DesiredMovementDirection, 1.0f);
    }

    // 6. Handle visual roll separately, based only on the left/right input
    if (!FMath::IsNearlyZero(MovementVector.X))
    {
        UpdateRotation(MovementVector.X, 0.0f); // Update visual roll
    }
    else
    {
        // Optional: Smoothly return roll to 0 if no left/right input is given
         UpdateRotation(0.0f, 0.0f);
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

// Update visual roll only
void ASpaceshipCharacter::UpdateRotation(float RollInput, float PitchInput)
{
    float TargetRoll = RollInput * MaxRollAngle;
    FRotator NewRotation = GetActorRotation();
    NewRotation.Roll = FMath::FInterpTo(NewRotation.Roll, TargetRoll, GetWorld()->GetDeltaSeconds(), 5.0f);
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

float ASpaceshipCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    // --- Color Matching Check ---
    ASoundSphere* Projectile = Cast<ASoundSphere>(DamageCauser);
    if (Projectile && Projectile->GetColor() == bIsWhiteMode)
    {
        return 0.0f; // Ignore damage if colors match
    }

    // --- Apply Actual Damage ---
    const float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

    // --- Trigger Hit Effects if Damage Was Taken ---
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

void ASpaceshipCharacter::UpdateThrusterParameters(float BaseForce)
{
    // Utiliser une courbe exponentielle pour la force de base
    // Cela donnera plus de nuances aux petits mouvements tout en permettant des effets dramatiques
    float NormalizedForce = FMath::Pow(BaseForce, 2.0f) * 100.0f;
    const float InterpSpeed = 5.0f;
    
    // Calculer les valeurs cibles
    if (LeftThrusterVFX && RightThrusterVFX)
    {
        for (UNiagaraComponent* Thruster : {LeftThrusterVFX, RightThrusterVFX})
        {
            if (BaseForce > 0.0f)
            {
                if (!Thruster->IsActive()) 
                {
                    Thruster->Activate(true);
                }
                
                // Application directe des multiplicateurs comme en BP
                Thruster->SetVariableFloat(ThrustersSpeedParam, BaseForce * THRUSTER_FORCE_MULTIPLIER);
                Thruster->SetVariableFloat(ParticulateSpeedParam, BaseForce * PARTICULATE_FORCE_MULTIPLIER);
                Thruster->SetVariableFloat(EnergyCoreSpeedParam, BaseForce * ENERGY_CORE_FORCE_MULTIPLIER);
                Thruster->SetVariableFloat(HeatHazeSpeedParam, BaseForce * HEAT_HAZE_FORCE_MULTIPLIER);
            }
            else
            {
                Thruster->Deactivate();
            }
        }
    }
    
}