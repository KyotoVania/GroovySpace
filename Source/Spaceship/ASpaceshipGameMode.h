#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "UWBP_HUD_Base.h"
#include "SpaceshipCharacter.h"
#include "AScoreManager.h"
#include "ASpaceshipGameMode.generated.h"

UCLASS()
class SPACESHIP_API ASpaceshipGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ASpaceshipGameMode();

	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;

	// Get the current HUD (with safety checks)
	UFUNCTION(BlueprintCallable, Category = "HUD")
	UWBP_HUD_Base* GetGameHUD() const;

	// Attempt to get the current spaceship character (with safety checks)
	UFUNCTION(BlueprintCallable, Category = "Character")
	ASpaceshipCharacter* GetSpaceshipCharacter() const;

	// Get the score manager
	UFUNCTION(BlueprintCallable, Category = "Score")
	AScoreManager* GetScoreManager() const { return ScoreManager; }

protected:
	virtual void BeginPlay() override;

	// Les classes à assigner dans le Blueprint
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Classes")
	TSubclassOf<APawn> SpaceshipCharacterClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Classes")
	TSubclassOf<UWBP_HUD_Base> GameHUDWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Classes")
	TSubclassOf<AScoreManager> ScoreManagerClass;

	// Reference to active HUD
	UPROPERTY()
	UWBP_HUD_Base* ActiveHUD;

	// Reference to score manager
	UPROPERTY()
	AScoreManager* ScoreManager;

	// Initialize the HUD for a player
	void InitializeHUDForPlayer(APlayerController* PlayerController);

	// Initialize the score manager
	void InitializeScoreManager();

private:
	// Safety check before HUD operations
	bool ValidateHUDOperation() const;
};