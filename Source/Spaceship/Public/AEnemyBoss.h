#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AScoreManager.h"
#include "Spaceship/ASpaceshipGameMode.h"
#include "AEnemyBoss.generated.h"

UCLASS()
class SPACESHIP_API AEnemyBoss : public AActor
{
	GENERATED_BODY()

public:
	AEnemyBoss();

	// Vie actuelle du boss
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss Stats")
	float CurrentHealth;

	// Vie maximale du boss
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss Stats")
	float MaxHealth;

	// Collision pour détecter les tirs
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Collision")
	class UBoxComponent* Hitbox;

	// Fonction appelée lorsqu’il subit des dégâts
	UFUNCTION(BlueprintCallable, Category = "Boss")
	float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser);

	void HandleDeath();
	// Fonction pour vérifier s’il est détruit
	UFUNCTION(BlueprintCallable, Category = "Boss")
	bool IsDestroyed() const;
	// Son joué quand le boss prend des dégâts
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	USoundBase* HitSound;

	// Son joué quand le boss est détruit
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	USoundBase* DeathSound;

protected:
	virtual void BeginPlay() override;
	ASpaceshipGameMode* GetSpaceshipGameMode() const;
	void OnBossHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	               FVector NormalImpulse,
	               const FHitResult& Hit);

};
