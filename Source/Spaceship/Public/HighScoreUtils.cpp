#include "HighScoreUtils.h"

void UHighScoreUtils::AddScore(FHighScoreSongSet& ScoreSet, USoundWave* Song, int32 NewScore, int32 Difficulty)
{
    if (!Song)
    {
        UE_LOG(LogTemp, Warning, TEXT("AddScore: Invalid song"));
        return;
    }

    TSoftObjectPtr<USoundWave> SongPtr(Song);
    FSongScores& Scores = ScoreSet.SongScores.FindOrAdd(SongPtr);

    // Create new score entry
    FScoreEntry NewEntry;
    NewEntry.Score = NewScore;
    NewEntry.Difficulty = Difficulty;
    NewEntry.DateAchieved = FDateTime::Now().ToString();

    // Update statistics
    Scores.TotalPlays++;
    Scores.TotalScore += NewScore;

    // Add to top scores and sort
    Scores.TopScores.Add(NewEntry);
    Scores.TopScores.Sort([](const FScoreEntry& A, const FScoreEntry& B) {
        return A.Score > B.Score;
    });

    // Keep only top 10 scores
    if (Scores.TopScores.Num() > 10)
    {
        Scores.TopScores.SetNum(10);
    }
}

int32 UHighScoreUtils::GetBestScore(const FHighScoreSongSet& ScoreSet, USoundWave* Song)
{
    if (!Song)
    {
        return 0;
    }

    TSoftObjectPtr<USoundWave> SongPtr(Song);
    const FSongScores* Scores = ScoreSet.SongScores.Find(SongPtr);
    
    if (!Scores || Scores->TopScores.Num() == 0)
    {
        return 0;
    }

    return Scores->TopScores[0].Score;
}

float UHighScoreUtils::GetAverageScore(const FHighScoreSongSet& ScoreSet, USoundWave* Song)
{
    if (!Song)
    {
        return 0.0f;
    }

    TSoftObjectPtr<USoundWave> SongPtr(Song);
    const FSongScores* Scores = ScoreSet.SongScores.Find(SongPtr);
    
    if (!Scores || Scores->TotalPlays == 0)
    {
        return 0.0f;
    }

    return static_cast<float>(Scores->TotalScore) / Scores->TotalPlays;
}

FString UHighScoreUtils::GetFormattedScoreList(const FHighScoreSongSet& ScoreSet, USoundWave* Song)
{
    if (!Song)
    {
        return TEXT("Invalid Song");
    }

    TSoftObjectPtr<USoundWave> SongPtr(Song);
    const FSongScores* Scores = ScoreSet.SongScores.Find(SongPtr);
    
    if (!Scores || Scores->TopScores.Num() == 0)
    {
        return TEXT("No scores available");
    }

    FString Result;
    for (int32 i = 0; i < Scores->TopScores.Num(); ++i)
    {
        const FScoreEntry& Entry = Scores->TopScores[i];
        Result += FString::Printf(TEXT("%d. %d pts (Diff: %d) - %s\n"), 
            i + 1, Entry.Score, Entry.Difficulty, *Entry.DateAchieved);
    }

    return Result;
}

bool UHighScoreUtils::HasMetScoreThreshold(const FHighScoreSongSet& ScoreSet, USoundWave* Song, int32 Threshold)
{
    return GetBestScore(ScoreSet, Song) >= Threshold;
}