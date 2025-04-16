#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"

#include "ALobbyGameMode.generated.h"

UCLASS()
class SPACESHIP_API ALobbyGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ALobbyGameMode();

	// Called when the player interacts with the spaceship in the lobby
	UFUNCTION(BlueprintCallable, Category = "Lobby")
	void OnSpaceshipInteraction();

	// Handle the transition to gameplay level
	UFUNCTION(BlueprintCallable, Category = "Lobby")
	void TransitionToGameplay();

protected:
	virtual void BeginPlay() override;

	// Cinematic sequence for spaceship entry
	UPROPERTY(EditDefaultsOnly, Category = "Cinematic")
	class ULevelSequence* SpaceshipEntrySequence;

	// Name of the gameplay level to load
	UPROPERTY(EditDefaultsOnly, Category = "Lobby")
	FName GameplayLevelName;

private:
	// Called when the entry sequence finishes
	UFUNCTION()
	void OnEntrySequenceFinished();
};