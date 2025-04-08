#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ASoundSphere.generated.h"

class USphereComponent;
class UStaticMeshComponent;

UCLASS()
class SPACESHIP_API ASoundSphere : public AActor
{
	GENERATED_BODY()

public:
	ASoundSphere();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
				   UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, 
				   bool bFromSweep, const FHitResult& SweepResult);
	void Reset();

	void FireInDirection(const FVector& ShootDirection);

	// ✅ Setter pour la couleur
	UFUNCTION(BlueprintCallable, Category = "SoundSphere")
	void SetMaterialColor(UMaterialInterface* NewMaterial);

	// ✅ Setter pour la vitesse
	UFUNCTION(BlueprintCallable, Category = "SoundSphere")
	void SetSpeed(float SpeedMultiplier);

	// ✅ Setter pour la direction
	UFUNCTION(BlueprintCallable, Category = "SoundSphere")
	void SetDirection(const FVector& Direction);
	// ✅ Matériaux pour changer la couleur
	UPROPERTY(EditAnywhere, Category = "SoundSphere")
	float TimerBeforeDestroy = 5.0f;
	// ✅ Stocke la couleur actuelle
	UPROPERTY(VisibleAnywhere, Category = "SoundSphere")
	bool bIsWhite = true; // Couleur par défaut : blanc
	// Ajoutez cette propriété
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SoundSphere")
	float MaxXPosition = 5000.0f; // Position X maximale avant désactivation
	
	UFUNCTION(BlueprintCallable, Category = "SoundSphere")
	bool GetColor() const { return bIsWhite; }

	UFUNCTION(BlueprintCallable, Category = "SoundSphere")
	void SetColor(bool bNewColor);
	void StopMovement();
	UPROPERTY()
	bool bIsActive = false;
private:
	FTimerHandle StopMovementTimerHandle;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	USphereComponent* CollisionComponent;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* MeshComponent;

	// ✅ Stocke la direction du mouvement
	FVector Velocity = FVector::ZeroVector;

	// ✅ Stocke la vitesse de déplacement
	float MoveSpeed = 1200.0f;


	float Damage = 10.0f;  // Dégâts infligés par le projectile

};
