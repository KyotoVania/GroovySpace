#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "uSpaceshipSaveGame.h"
#include "SpaceshipSaveManager.generated.h"

/**
 * Manager class to handle save/load operations
 */
UCLASS(Blueprintable, BlueprintType)
class SPACESHIP_API USpaceshipSaveManager : public UObject
{
	GENERATED_BODY()

public:
	USpaceshipSaveManager();

	// Save slot name
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save")
	FString SaveSlotName;

	// User index (typically 0)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save")
	int32 UserIndex;

	// Current save game instance
	UPROPERTY(BlueprintReadOnly, Category = "Save")
	TObjectPtr<USpaceshipSaveGame> CurrentSave;

	// Load save game data
	UFUNCTION(BlueprintCallable, Category = "Save")
	bool LoadGame();

	// Save current game data
	UFUNCTION(BlueprintCallable, Category = "Save")
	bool SaveGame();
	UFUNCTION(BlueprintCallable, Category = "Save")
	bool IsFirstTime() const;

	UFUNCTION(BlueprintCallable, Category = "Save")
	void SetFirstTimeFlag(bool bNewValue);
	// Get or create the save manager (singleton-like pattern)
	UFUNCTION(BlueprintCallable, Category = "Save", meta = (WorldContext = "WorldContextObject"))
	static USpaceshipSaveManager* GetSaveManager(UObject* WorldContextObject);

	// Add a high score for a song
	UFUNCTION(BlueprintCallable, Category = "Scores")
	void AddHighScore(USoundWave* Song, int32 Score);

	// Get the best score for a song
	UFUNCTION(BlueprintCallable, Category = "Scores")
	int32 GetBestScore(USoundWave* Song) const;

	// Get formatted high score list for a song
	UFUNCTION(BlueprintCallable, Category = "Scores")
	FString GetFormattedHighScores(USoundWave* Song) const;

private:
	// Create a new save game
	USpaceshipSaveGame* CreateNewSave();

	// Static instance for singleton pattern
	static TWeakObjectPtr<USpaceshipSaveManager> Instance;
};