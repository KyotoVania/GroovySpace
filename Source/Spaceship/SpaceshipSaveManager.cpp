#include "SpaceshipSaveManager.h"
#include "Kismet/GameplayStatics.h"
#include "HighScoreUtils.h"
#include "USpaceshipSaveGame.h"
#include "Engine/World.h"

// Initialize static instance
TWeakObjectPtr<USpaceshipSaveManager> USpaceshipSaveManager::Instance = nullptr;

USpaceshipSaveManager::USpaceshipSaveManager()
{
    SaveSlotName = TEXT("SpaceshipSave");
    UserIndex = 0;
    CurrentSave = nullptr;
}

bool USpaceshipSaveManager::LoadGame()
{
    // Check if save exists
    if (UGameplayStatics::DoesSaveGameExist(SaveSlotName, UserIndex))
    {
        // Load saved game
        USaveGame* LoadedGame = UGameplayStatics::LoadGameFromSlot(SaveSlotName, UserIndex);
        CurrentSave = Cast<USpaceshipSaveGame>(LoadedGame);
        
        if (CurrentSave)
        {
            UE_LOG(LogTemp, Log, TEXT("Game loaded successfully"));
            return true;
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Failed to load save game or invalid save data"));
        }
    }
    
    // Create new save game if loading failed
    CurrentSave = CreateNewSave();
    return false;
}

bool USpaceshipSaveManager::SaveGame()
{
    if (!CurrentSave)
    {
        CurrentSave = CreateNewSave();
    }
    
    return UGameplayStatics::SaveGameToSlot(CurrentSave, SaveSlotName, UserIndex);
}

USpaceshipSaveManager* USpaceshipSaveManager::GetSaveManager(UObject* WorldContextObject)
{
    // Return existing instance if valid
    if (Instance.IsValid())
    {
        return Instance.Get();
    }
    
    // Create new instance
    UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
    if (!World)
    {
        return nullptr;
    }
    
    USpaceshipSaveManager* Manager = NewObject<USpaceshipSaveManager>(World);
    Manager->LoadGame(); // Load existing save or create new one
    Instance = Manager;
    
    // Add to root to prevent garbage collection
    //Manager->AddToRoot();
    
    return Manager;
}

USpaceshipSaveGame* USpaceshipSaveManager::CreateNewSave()
{
    USpaceshipSaveGame* NewSave = Cast<USpaceshipSaveGame>(UGameplayStatics::CreateSaveGameObject(USpaceshipSaveGame::StaticClass()));
    if (NewSave)
    {
        // Default values are set in the constructor
        UE_LOG(LogTemp, Log, TEXT("Created new save game"));
    }
    return NewSave;
}

void USpaceshipSaveManager::AddHighScore(USoundWave* Song, int32 Score)
{
    if (!CurrentSave || !Song)
    {
        return;
    }
    
    // Update last score and song
    CurrentSave->LastScore = Score;
    CurrentSave->LastSong = Song;
    
    // Add score to high scores
    UHighScoreUtils::AddHighScoreToSet(CurrentSave->SongHighScores, Song, Score);
    
    // Save changes
    SaveGame();
}

int32 USpaceshipSaveManager::GetBestScore(USoundWave* Song) const
{
    if (!CurrentSave || !Song)
    {
        return 0;
    }
    
    return UHighScoreUtils::GetBestScoreForSong(CurrentSave->SongHighScores, Song);
}

FString USpaceshipSaveManager::GetFormattedHighScores(USoundWave* Song) const
{
    if (!CurrentSave || !Song)
    {
        return TEXT("No scores available");
    }
    
    return UHighScoreUtils::GetFormattedHighScoreList(CurrentSave->SongHighScores, Song);
}