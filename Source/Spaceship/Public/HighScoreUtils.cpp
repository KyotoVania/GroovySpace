#include "HighScoreUtils.h"

void UHighScoreUtils::AddHighScoreToSet(FHighScoreSoungSet& ScoreSet, USoundWave* Song, int32 NewScore)
{
	if (!Song)
	{
		UE_LOG(LogTemp, Warning, TEXT("AddHighScoreToSet: Song is null! Cannot add score."));
		return;
	}

	//verifier si le scoreset est null
	if (!ScoreSet.HighScores.Num())
	{
		//si c'est le cas, on crée un nouveau score
		FHighScoreList NewList;
		NewList.Scores.Add(NewScore);
		ScoreSet.HighScores.Add(Song, NewList);
		UE_LOG(LogTemp, Log, TEXT("Added new song entry with score: %d"), NewScore);
		return;
	}
	
	// Vérifie si la chanson est déjà enregistrée
	FHighScoreList* ScoreList = ScoreSet.HighScores.Find(Song);
    
	if (!ScoreList)
	{
		// Si elle n'existe pas, on la crée
		FHighScoreList NewList;
		NewList.Scores.Add(NewScore);
		ScoreSet.HighScores.Add(Song, NewList);
		UE_LOG(LogTemp, Log, TEXT("Added new song entry with score: %d"), NewScore);
	}
	else
	{
		// Ajouter le score et trier en ordre décroissant
		ScoreList->Scores.Add(NewScore);
		ScoreList->Scores.Sort([](const int32& A, const int32& B) {
			return A > B; // Trie du plus grand au plus petit
		});

		// S'assurer qu'on ne garde que les 10 meilleurs scores
		if (ScoreList->Scores.Num() > 10)
		{
			ScoreList->Scores.SetNum(10);
		}
		//for debug print the final list
		for (int32 i = 0; i < ScoreList->Scores.Num(); ++i)
		{
			UE_LOG(LogTemp, Log, TEXT("Score %d: %d"), i, ScoreList->Scores[i]);
		}

		UE_LOG(LogTemp, Log, TEXT("Added score: %d and sorted list to song : %s"), NewScore, *Song->GetName());
	}
}


FString UHighScoreUtils::GetFormattedHighScoreList(const FHighScoreSoungSet& ScoreSet, USoundWave* Song)
{
	if (!Song)
	{
		return TEXT("Invalid Song");
	}

	const FHighScoreList* ScoreList = ScoreSet.HighScores.Find(Song);
	if (!ScoreList)
	{
		return TEXT("No scores available for this song.");
	}

	FString Result;
	for (int32 i = 0; i < ScoreList->Scores.Num(); ++i)
	{
		Result += FString::Printf(TEXT("%d: %d\n"), i + 1, ScoreList->Scores[i]);
	}

	return Result;
}
int64 UHighScoreUtils::GetBestScoreForSong(const FHighScoreSoungSet& ScoreSet, USoundWave* Song)
{
	if (!Song)
	{
		UE_LOG(LogTemp, Warning, TEXT("GetBestScoreForSong: Song is null!"));
		return -1; // Valeur invalide si le song est null
	}

	const FHighScoreList* ScoreList = ScoreSet.HighScores.Find(Song);
	if (!ScoreList || ScoreList->Scores.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("No scores available for song: %s"), *Song->GetName());
		return -1; // Aucune donnée pour ce son
	}

	// Le premier élément est le meilleur car la liste est triée décroissante
	int32 BestScore = ScoreList->Scores[0];

	UE_LOG(LogTemp, Log, TEXT("Best score for song %s: %d"), *Song->GetName(), BestScore);
	return BestScore;
}
