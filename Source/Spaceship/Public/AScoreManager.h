#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Spaceship/SpaceshipCharacter.h"
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
	ASpaceshipCharacter* Character;

	UFUNCTION(BlueprintCallable, Category = "Combo")
	void AddScore(int32 Points);

	// Fonction pour gérer le combo
	UFUNCTION(BlueprintCallable, Category = "Combo")
	void IncrementCombo();

	UFUNCTION(BlueprintCallable, Category = "Combo")
	void BreakCombo();

	UFUNCTION(BlueprintCallable, Category = "Score")
	int32 GetScore() const { return CurrentScore; }

private:

};
