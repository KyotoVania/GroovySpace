#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Sound/SoundWave.h"
#include "FHighScoreSoungSet.h"
#include "HighScoreUtils.generated.h"

UCLASS()
class SPACESHIP_API UHighScoreUtils : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "HighScore")
	static FString GetFormattedHighScoreList(const FHighScoreSoungSet& ScoreSet, USoundWave* Song);

	// Fonction pour ajouter un score à une chanson et trier la liste
	UFUNCTION(BlueprintCallable, Category = "HighScore")
	static void AddHighScoreToSet(UPARAM(ref) FHighScoreSoungSet& ScoreSet, USoundWave* Song, int32 NewScore);

	UFUNCTION(BlueprintCallable, Category = "HighScore")
	static int64 GetBestScoreForSong(const FHighScoreSoungSet& ScoreSet, USoundWave* Song);

};

