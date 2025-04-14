#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NiagaraProjectile.generated.h"

// Forward Declarations
class USphereComponent;
class UProjectileMovementComponent;
class UNiagaraComponent;
class UNiagaraSystem;
class USoundBase;
USTRUCT(BlueprintType)
struct FNiagaraEffectPair
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
    UNiagaraSystem* ProjectileFX = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
    UNiagaraSystem* ImpactFX = nullptr;
};

UCLASS()
class SPACESHIP_API ANiagaraProjectile : public AActor
{
    GENERATED_BODY()

public:
    ANiagaraProjectile();

    // --- Components ---

    // Collision sphere
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    USphereComponent* CollisionComp;

    // Main visual effect component (attached)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UNiagaraComponent* NiagaraComp;

    // Handles movement physics
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UProjectileMovementComponent* ProjectileMovement;

    // --- Properties ---

    // Visual effect asset to use for the projectile trail/body
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effects")
    FNiagaraEffectPair NiagaraEffects; // Renommé pour plus de clarté
    
    // Sound to play on impact
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Audio")
    USoundBase* ImpactSound;

    // Damage dealt by the projectile
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Gameplay")
    float Damage = 20.0f;

    // Initial speed set on the ProjectileMovementComponent
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement")
    float InitialSpeed = 3000.0f;

    // Max speed set on the ProjectileMovementComponent
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement")
    float MaxSpeed = 3000.0f;

    // Time after which the projectile destroys itself
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Gameplay")
    float Lifespan = 3.0f;

    // Optional: To know if it's white/black if needed for rules
    bool bIsWhite = true;

protected:
    virtual void BeginPlay() override;

    // Function called when projectile hits something
    UFUNCTION()
    void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

public:
    // Function for the Spawner (SpaceshipCharacter) to set the color mode
    void SetColorMode(bool bNewIsWhite);

    // Optional: Function to set the actual NiagaraSystem asset dynamically if needed
    void SetNiagaraAsset(UNiagaraSystem* NewFX);
};