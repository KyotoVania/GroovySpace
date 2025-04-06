#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Sound/SoundWave.h"
#include "UVisualizerManager.h"
#include "FHighScoreSongSet.h"
#include "uSpaceshipSaveGame.generated.h"

/**
 * Save game class for storing player progress and settings
 */
UCLASS()
class SPACESHIP_API USpaceshipSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	USpaceshipSaveGame();

	// Player projectile skin ID
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player")
	int32 ProjectileSkinID;

	// Color skin ID for boss and player spaceship
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player")
	int32 ColorSkinID;

	// High scores per song
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scores")
	FHighScoreSongSet SongHighScores;

	// Game difficulty (1-10)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game", meta = (ClampMin = "1", ClampMax = "10"))
	int32 Difficulty;

	// Visualizer shape
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualizer")
	EVisualizerShape VisualizerShape;

	// Flag for first time playing (for cinematics)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game")
	bool bFirstTime;

	// Last game score (consider removing as noted)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scores")
	int32 LastScore;
    
	// Last played song
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	TSoftObjectPtr<USoundWave> LastSong;
};