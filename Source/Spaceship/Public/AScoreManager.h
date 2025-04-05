// AScoreManager.h with fixed TakeDamage to avoid conflict
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AScoreManager.generated.h"

UCLASS()
class SPACESHIP_API AScoreManager : public AActor
{
	GENERATED_BODY()

public:
	AScoreManager();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Score")
	int32 CurrentScore;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combo")
	int32 ComboCount;

	UFUNCTION(BlueprintCallable, Category = "Combo")
	void AddScore(int32 Points);

	// Function for managing combo
	UFUNCTION(BlueprintCallable, Category = "Combo")
	void IncrementCombo();

	UFUNCTION(BlueprintCallable, Category = "Combo")
	void BreakCombo();

	UFUNCTION(BlueprintCallable, Category = "Score")
	int32 GetScore() const { return CurrentScore; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float CurrentHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float MaxHealth = 100.0f;

	UFUNCTION(BlueprintCallable, Category = "Health")
	void UpdateHealth(float Health);

private:
	bool bIsInvincible;
	float InvincibilityDuration;
	//timer handle for invincibility
	FTimerHandle InvincibilityTimerHandle;

	void ResetInvincibility();

};