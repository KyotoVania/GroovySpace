#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Sound/SoundWave.h"
#include "FHighScoreSongSet.h"
#include "HighScoreUtils.generated.h"

UCLASS()
class SPACESHIP_API UHighScoreUtils : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "High Score")
	static void AddScore(UPARAM(ref) FHighScoreSongSet& ScoreSet, 
						USoundWave* Song, 
						int32 NewScore, 
						int32 Difficulty);

	UFUNCTION(BlueprintCallable, Category = "High Score")
	static int32 GetBestScore(const FHighScoreSongSet& ScoreSet, USoundWave* Song);

	UFUNCTION(BlueprintCallable, Category = "High Score")
	static float GetAverageScore(const FHighScoreSongSet& ScoreSet, USoundWave* Song);

	UFUNCTION(BlueprintCallable, Category = "High Score")
	static FString GetFormattedScoreList(const FHighScoreSongSet& ScoreSet, USoundWave* Song);

	UFUNCTION(BlueprintCallable, Category = "High Score")
	static bool HasMetScoreThreshold(const FHighScoreSongSet& ScoreSet, USoundWave* Song, int32 Threshold);
};