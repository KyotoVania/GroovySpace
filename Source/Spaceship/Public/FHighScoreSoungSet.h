#pragma once

#include "CoreMinimal.h"
#include "Sound/SoundWave.h"
#include "FHighScoreSoungSet.generated.h"

USTRUCT(BlueprintType)
struct FHighScoreList
{
	GENERATED_BODY()

public:
	// Tableau de scores
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<int32> Scores;
};

USTRUCT(BlueprintType)
struct FHighScoreSoungSet
{
	GENERATED_BODY()

public:
	// Map entre un SoundWave et une liste de scores
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<USoundWave*, FHighScoreList> HighScores;
	
};

void AddHighScoreToSet(FHighScoreSoungSet& ScoreSet, USoundWave* Song, int32 NewScore);
