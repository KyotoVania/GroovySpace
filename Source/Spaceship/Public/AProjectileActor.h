#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "AProjectileActor.generated.h"

UCLASS()
class SPACESHIP_API AProjectileActor : public AActor
{
	GENERATED_BODY()

public:
	AProjectileActor();

	// Propriétés supplémentaires
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
	float Speed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
	float Lifetime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
	class USoundBase* ImpactSound;

	
	
	// Pool Manager
	UPROPERTY()
	class AProjectilePoolManager* PoolManager;

	// Méthodes
	UFUNCTION(BlueprintCallable, Category = "Projectile")
	void ActivateProjectile(const FVector& SpawnLocation, const FRotator& SpawnRotation, float GivenSpeed);
	UFUNCTION(BlueprintCallable, Category = "Projectile")
	void DeactivateProjectile();

protected:
	virtual void BeginPlay() override;
	
};
