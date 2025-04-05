#pragma once

#include "CoreMinimal.h"
#include "BaseSelectorComponent.h"
#include "Sound/SoundWave.h"
#include "UVisualizerManager.h"
#include "SongSelectorComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSongChanged, USoundWave*, NewSong);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDifficultyChanged, int32, NewDifficulty);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnVisualizerShapeChanged, EVisualizerShape, NewShape);

/**
 * Component for selecting songs, difficulty, and visualizer shape
 */
UCLASS(ClassGroup=(UI), meta=(BlueprintSpawnableComponent))
class SPACESHIP_API USongSelectorComponent : public UBaseSelectorComponent
{
    GENERATED_BODY()

public:
    USongSelectorComponent();

    // Available songs
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Songs")
    TArray<TObjectPtr<USoundWave>> AvailableSongs;

    // Song selection changed delegate
    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnSongChanged OnSongChanged;

    // Difficulty changed delegate
    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnDifficultyChanged OnDifficultyChanged;

    // Visualizer shape changed delegate
    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnVisualizerShapeChanged OnVisualizerShapeChanged;

    // Get current song
    UFUNCTION(BlueprintCallable, Category = "Songs")
    USoundWave* GetCurrentSong() const;

    // Get current difficulty
    UFUNCTION(BlueprintCallable, Category = "Difficulty")
    int32 GetCurrentDifficulty() const;

    // Get current visualizer shape
    UFUNCTION(BlueprintCallable, Category = "Visualizer")
    EVisualizerShape GetCurrentVisualizerShape() const;

    // Get high score for current song
    UFUNCTION(BlueprintCallable, Category = "Scores")
    int32 GetCurrentSongHighScore() const;

    // Increment song selection
    UFUNCTION(BlueprintCallable, Category = "Selection")
    void NextSong();

    // Decrement song selection
    UFUNCTION(BlueprintCallable, Category = "Selection")
    void PreviousSong();

    // Increment difficulty
    UFUNCTION(BlueprintCallable, Category = "Selection")
    void IncreaseDifficulty();

    // Decrement difficulty
    UFUNCTION(BlueprintCallable, Category = "Selection")
    void DecreaseDifficulty();

    // Cycle visualizer shape
    UFUNCTION(BlueprintCallable, Category = "Selection")
    void CycleVisualizerShape();

protected:
    virtual void BeginPlay() override;

private:
    // Current song index
    int32 CurrentSongIndex;

    // Update the save file with current selection
    void UpdateSaveData();
};