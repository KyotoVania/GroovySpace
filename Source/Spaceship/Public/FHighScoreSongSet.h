#pragma once

#include "CoreMinimal.h"
#include "Sound/SoundWave.h"
#include "FHighScoreSongSet.generated.h"

USTRUCT(BlueprintType)
struct FScoreEntry
{
	GENERATED_BODY()

	// Score value
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Score = 0;

	// Difficulty when achieved
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Difficulty = 5;

	// Date achieved (stored as FDateTime::ToString())
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString DateAchieved;
};

USTRUCT(BlueprintType)
struct FSongScores
{
	GENERATED_BODY()

	// Top scores for this song
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FScoreEntry> TopScores;

	// Total plays
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 TotalPlays = 0;

	// Total score (for average calculation)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int64 TotalScore = 0;
};

USTRUCT(BlueprintType)
struct FHighScoreSongSet
{
	GENERATED_BODY()

	// Map between songs and their score data
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<TSoftObjectPtr<USoundWave>, FSongScores> SongScores;
};