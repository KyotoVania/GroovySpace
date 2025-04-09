#include "SpaceshipCharacter.h"

#include "Components/ArrowComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "ASoundSphere.h"
#include "EngineUtils.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

ASpaceshipCharacter::ASpaceshipCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    
    USkeletalMeshComponent* MeshRef = GetMesh();
    if (!MeshRef) return;

    // Create spawn points
    ProjectileSpawnPointWhite = CreateDefaultSubobject<UArrowComponent>(TEXT("ProjectileSpawnWhite"));
    ProjectileSpawnPointWhite->SetupAttachment(MeshRef);

    ProjectileSpawnPointBlack = CreateDefaultSubobject<UArrowComponent>(TEXT("ProjectileSpawnBlack"));
    ProjectileSpawnPointBlack->SetupAttachment(MeshRef);

    // Configure character movement
    GetCharacterMovement()->bOrientRotationToMovement = false;
    GetCharacterMovement()->bUseControllerDesiredRotation = false;
    GetCharacterMovement()->DefaultLandMovementMode = MOVE_Flying;
    GetCharacterMovement()->MaxFlySpeed = 1000.0f;
}

void ASpaceshipCharacter::BeginPlay()
{
    Super::BeginPlay();
    
    SaveManager = USpaceshipSaveManager::GetSaveManager(GetWorld());
    
    for (TActorIterator<AObjectPoolManager> It(GetWorld()); It; ++It)
    {
        PoolManager = *It;
        break;
    }
    
    UpdateMaterials();
}

void ASpaceshipCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (bIsInSpaceship)
    {
        HandleSpaceshipMovement(DeltaTime);
    }
}

void ASpaceshipCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    // Get the player controller
    APlayerController* PC = Cast<APlayerController>(GetController());
    if (PC)
    {
        // Get the local player enhanced input subsystem
        UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer());
        if (Subsystem)
        {
            // Add the mapping context
            Subsystem->AddMappingContext(DefaultMappingContext, 0);
        }
    }

    // Cast to enhanced input component
    UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);
    if (EnhancedInputComponent)
    {
        // Bind the actions
        EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ASpaceshipCharacter::Move);
        EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Triggered, this, &ASpaceshipCharacter::HandleFire);
        EnhancedInputComponent->BindAction(SwitchModeAction, ETriggerEvent::Triggered, this, &ASpaceshipCharacter::HandleSwitchMode);
        EnhancedInputComponent->BindAction(ExitShipAction, ETriggerEvent::Triggered, this, &ASpaceshipCharacter::HandleExitShip);
    }
}
void ASpaceshipCharacter::Move(const FInputActionValue& Value)
{
    if (!bIsInSpaceship) return;

    // Get the input vector
    FVector2D MovementVector = Value.Get<FVector2D>();

    // Handle forward/backward movement
    if (!FMath::IsNearlyZero(MovementVector.X))
    {
        AddMovementInput(GetActorForwardVector(), MovementVector.X * ThrustForce);
    }

    // Handle right/left movement
    if (!FMath::IsNearlyZero(MovementVector.Y))
    {
        AddMovementInput(GetActorRightVector(), MovementVector.Y);
        UpdateRotation(MovementVector.Y, 0.0f);
    }
}

void ASpaceshipCharacter::HandleFire(const FInputActionValue& Value)
{
    FireProjectile();
}

void ASpaceshipCharacter::HandleSwitchMode(const FInputActionValue& Value)
{
    ToggleColor();
}

void ASpaceshipCharacter::HandleExitShip(const FInputActionValue& Value)
{
    ToggleSpaceshipMode();
}

void ASpaceshipCharacter::UpdateRotation(float RollInput, float PitchInput)
{
    float TargetRoll = RollInput * MaxRollAngle;
    FRotator NewRotation = GetActorRotation();
    NewRotation.Roll = FMath::FInterpTo(NewRotation.Roll, TargetRoll, GetWorld()->GetDeltaSeconds(), 5.0f);
    SetActorRotation(NewRotation);
}

void ASpaceshipCharacter::HandleSpaceshipMovement(float DeltaTime)
{
    // Additional spaceship-specific movement logic can be added here
    // Such as momentum, drag, etc.
}

void ASpaceshipCharacter::FireProjectile()
{
    if (!bCanFire || !PoolManager || !SkinOptions) return;

    UArrowComponent* SpawnPoint = bIsWhiteMode ? ProjectileSpawnPointWhite : ProjectileSpawnPointBlack;
    if (!SpawnPoint) return;

    ASoundSphere* Projectile = Cast<ASoundSphere>(PoolManager->GetPooledObject());
    if (!Projectile) return;

    int32 SkinIndex = SaveManager->CurrentSave->ColorSkinID;
    if (!SkinOptions->AvailableSkins.IsValidIndex(SkinIndex)) return;

    const FSkinOption& CurrentSkin = SkinOptions->AvailableSkins[SkinIndex];
    const FMaterialSet& Materials = CurrentSkin.ProjectileMaterialSets;

    Projectile->SetActorLocation(SpawnPoint->GetComponentLocation());
    Projectile->SetDirection(SpawnPoint->GetForwardVector());
    Projectile->SetColor(bIsWhiteMode);
    Projectile->SetMaterialColor(bIsWhiteMode ? Materials.WhiteMaterial : Materials.BlackMaterial);

    bCanFire = false;
    GetWorld()->GetTimerManager().SetTimer(FireCooldownTimer, [this]() { bCanFire = true; }, FireCooldown, false);
}

void ASpaceshipCharacter::ToggleColor()
{
    bIsWhiteMode = !bIsWhiteMode;
    UpdateMaterials();
}

void ASpaceshipCharacter::ToggleSpaceshipMode()
{
    bIsInSpaceship = !bIsInSpaceship;
    
    // Update movement component settings
    UCharacterMovementComponent* Movement = GetCharacterMovement();
    if (Movement)
    {
        if (bIsInSpaceship)
        {
            Movement->SetMovementMode(MOVE_Flying);
        }
        else
        {
            Movement->SetMovementMode(MOVE_Walking);
        }
    }
}

float ASpaceshipCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    ASoundSphere* Projectile = Cast<ASoundSphere>(DamageCauser);
    if (Projectile && Projectile->GetColor() == bIsWhiteMode)
    {
        return 0.0f; // Ignore damage if colors match
    }

    return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
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

    for (int32 i = 0; i < MeshRef->GetNumMaterials(); i++)
    {
        MeshRef->SetMaterial(i, bIsWhiteMode ? Materials.WhiteMaterial : Materials.BlackMaterial);
    }
}

void ASpaceshipCharacter::Enter(ACharacter* Character)
{
    if (!Character) return;

    // Store the reference to the character
    PlayerCharacter = Character;

    // Get the controller of the character
    AController* ControllerRef = Character->GetController();
    if (ControllerRef)
    {
        // Possess the spaceship
        ControllerRef->Possess(this);
    }

    // Hide the character
    Character->SetActorHiddenInGame(true);
    Character->SetActorEnableCollision(false);
    Character->SetActorTickEnabled(false);
}

void ASpaceshipCharacter::Exit()
{
    if (!PlayerCharacter) return;

    // Get the controller of the spaceship
    AController* ControllerRef = GetController();
    if (ControllerRef)
    {
        // Possess the original character
        ControllerRef->Possess(PlayerCharacter);
    }

    // Unhide the character
    PlayerCharacter->SetActorHiddenInGame(false);
    PlayerCharacter->SetActorEnableCollision(true);
    PlayerCharacter->SetActorTickEnabled(true);

    // Clear the reference
    PlayerCharacter = nullptr;
}