#include "SongSelectorComponent.h"

USongSelectorComponent::USongSelectorComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    CurrentSongIndex = 0;
}

void USongSelectorComponent::BeginPlay()
{
    Super::BeginPlay();
    
    // Ensure we have at least one song
    if (AvailableSongs.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("No songs defined in SongSelectorComponent"));
    }
    
    // Initialize current song index
    if (SaveManager && SaveManager->CurrentSave && SaveManager->CurrentSave->LastSong)
    {
        // Try to find the last played song in the available songs
        for (int32 i = 0; i < AvailableSongs.Num(); ++i)
        {
            if (AvailableSongs[i] == SaveManager->CurrentSave->LastSong)
            {
                CurrentSongIndex = i;
                break;
            }
        }
    }
    
    // Ensure difficulty is within bounds
    if (SaveManager && SaveManager->CurrentSave)
    {
        SaveManager->CurrentSave->Difficulty = FMath::Clamp(SaveManager->CurrentSave->Difficulty, 1, 10);
    }
}

USoundWave* USongSelectorComponent::GetCurrentSong() const
{
    if (AvailableSongs.Num() == 0)
    {
        return nullptr;
    }
    
    int32 SafeIndex = FMath::Clamp(CurrentSongIndex, 0, AvailableSongs.Num() - 1);
    return AvailableSongs[SafeIndex];
}

int32 USongSelectorComponent::GetCurrentDifficulty() const
{
    if (!SaveManager || !SaveManager->CurrentSave)
    {
        return 5; // Default difficulty
    }
    
    return SaveManager->CurrentSave->Difficulty;
}

EVisualizerShape USongSelectorComponent::GetCurrentVisualizerShape() const
{
    if (!SaveManager || !SaveManager->CurrentSave)
    {
        return EVisualizerShape::Circle; // Default shape
    }
    
    return SaveManager->CurrentSave->VisualizerShape;
}

int32 USongSelectorComponent::GetCurrentSongHighScore() const
{
    USoundWave* CurrentSong = GetCurrentSong();
    if (!CurrentSong)
    {
        return 0;
    }
    
    if (!SaveManager)
    {
        return 0;
    }
    
    return SaveManager->GetBestScore(CurrentSong);
}

void USongSelectorComponent::NextSong()
{
    if (AvailableSongs.Num() == 0)
    {
        return;
    }
    
    CurrentSongIndex = IncrementIndex(CurrentSongIndex, AvailableSongs.Num());
    
    // Update save data
    if (SaveManager && SaveManager->CurrentSave)
    {
        SaveManager->CurrentSave->LastSong = GetCurrentSong();
        UpdateSaveData();
    }
    
    // Broadcast delegate
    OnSongChanged.Broadcast(GetCurrentSong());
    PlaySelectionChangedSound();
}

void USongSelectorComponent::PreviousSong()
{
    if (AvailableSongs.Num() == 0)
    {
        return;
    }
    
    CurrentSongIndex = DecrementIndex(CurrentSongIndex, AvailableSongs.Num());
    
    // Update save data
    if (SaveManager && SaveManager->CurrentSave)
    {
        SaveManager->CurrentSave->LastSong = GetCurrentSong();
        UpdateSaveData();
    }
    
    // Broadcast delegate
    OnSongChanged.Broadcast(GetCurrentSong());
    PlaySelectionChangedSound();
}

void USongSelectorComponent::IncreaseDifficulty()
{
    if (!SaveManager || !SaveManager->CurrentSave)
    {
        return;
    }
    
    // Increase difficulty (max 10)
    int32 NewDifficulty = FMath::Clamp(SaveManager->CurrentSave->Difficulty + 1, 1, 10);
    SaveManager->CurrentSave->Difficulty = NewDifficulty;
    UpdateSaveData();
    
    // Broadcast delegate
    OnDifficultyChanged.Broadcast(NewDifficulty);
    PlaySelectionChangedSound();
}

void USongSelectorComponent::DecreaseDifficulty()
{
    if (!SaveManager || !SaveManager->CurrentSave)
    {
        return;
    }
    
    // Decrease difficulty (min 1)
    int32 NewDifficulty = FMath::Clamp(SaveManager->CurrentSave->Difficulty - 1, 1, 10);
    SaveManager->CurrentSave->Difficulty = NewDifficulty;
    UpdateSaveData();
    // Broadcast delegate
    OnDifficultyChanged.Broadcast(NewDifficulty);
    PlaySelectionChangedSound();
}

void USongSelectorComponent::UpdateSaveData()
{
    if (SaveManager)
    {
        SaveManager->SaveGame();
    }
}

void USongSelectorComponent::CycleVisualizerShape()
{
    if (!SaveManager || !SaveManager->CurrentSave)
    {
        return;
    }
    
    // Cycle through visualizer shapes
    int32 CurrentShapeIndex = static_cast<int32>(SaveManager->CurrentSave->VisualizerShape);
    int32 NewShapeIndex = (CurrentShapeIndex + 1) % static_cast<int32>(EVisualizerShape::Max);
    
    SaveManager->CurrentSave->VisualizerShape = static_cast<EVisualizerShape>(NewShapeIndex);
    UpdateSaveData();
    
    // Broadcast delegate
    OnVisualizerShapeChanged.Broadcast(SaveManager->CurrentSave->VisualizerShape);
    PlaySelectionChangedSound();
}
