#include "SpaceshipCharacter.h"

#include "Components/ArrowComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "ASoundSphere.h"
#include "EngineUtils.h"

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

    PlayerInputComponent->BindAxis("MoveForward", this, &ASpaceshipCharacter::MoveForward);
    PlayerInputComponent->BindAxis("MoveRight", this, &ASpaceshipCharacter::MoveRight);
}

void ASpaceshipCharacter::MoveForward(float Value)
{
    if (bIsInSpaceship && !FMath::IsNearlyZero(Value))
    {
        AddMovementInput(GetActorForwardVector(), Value * ThrustForce);
    }
}

void ASpaceshipCharacter::MoveRight(float Value)
{
    if (bIsInSpaceship && !FMath::IsNearlyZero(Value))
    {
        AddMovementInput(GetActorRightVector(), Value);
        UpdateRotation(Value, 0.0f);
    }
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