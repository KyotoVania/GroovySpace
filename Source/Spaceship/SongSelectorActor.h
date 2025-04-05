#pragma once

#include "CoreMinimal.h"
#include "BaseSelectorActor.h"
#include "Sound/SoundWave.h"
#include "UVisualizerManager.h"
#include "Components/AudioComponent.h"
#include "SongSelectorActor.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSongChanged, USoundWave*, NewSong);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDifficultyChanged, int32, NewDifficulty);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnVisualizerShapeChanged, EVisualizerShape, NewShape);

/**
 * Actor for selecting songs, difficulty, and visualizer shape
 */
UCLASS()
class SPACESHIP_API ASongSelectorActor : public ABaseSelectorActor
{
    GENERATED_BODY()

public:
    ASongSelectorActor();

    // Available songs
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Songs")
    TArray<USoundWave*> AvailableSongs;

    // Song selection changed delegate
    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnSongChanged OnSongChanged;

    // Difficulty changed delegate
    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnDifficultyChanged OnDifficultyChanged;

    // Visualizer shape changed delegate
    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnVisualizerShapeChanged OnVisualizerShapeChanged;
    
    // UI Components
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UTextRenderComponent* SongTitleText;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UTextRenderComponent* DifficultyText;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UTextRenderComponent* HighScoreText;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UTextRenderComponent* VisualizerShapeText;

    // Preview components
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UAudioComponent* SongPreviewAudio;
    
    // Additional input actions
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    class UInputAction* NextSongAction;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    class UInputAction* PrevSongAction;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    class UInputAction* IncreaseDifficultyAction;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    class UInputAction* DecreaseDifficultyAction;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    class UInputAction* CycleVisualizerShapeAction;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    class UInputAction* PlayPreviewAction;

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

    // Song navigation
    UFUNCTION(BlueprintCallable, Category = "Selection")
    void NextSong();

    UFUNCTION(BlueprintCallable, Category = "Selection")
    void PreviousSong();

    // Difficulty adjustment
    UFUNCTION(BlueprintCallable, Category = "Selection")
    void IncreaseDifficulty();

    UFUNCTION(BlueprintCallable, Category = "Selection")
    void DecreaseDifficulty();

    // Visualizer shape cycling
    UFUNCTION(BlueprintCallable, Category = "Selection")
    void CycleVisualizerShape();

    // Confirm selection
    UFUNCTION(BlueprintCallable, Category = "Selection")
    void ConfirmSelection();

    // Preview functions
    UFUNCTION(BlueprintCallable, Category = "Preview")
    void StartSongPreview();

    UFUNCTION(BlueprintCallable, Category = "Preview")
    void StopSongPreview();

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    
    // Override UpdateUI to refresh the visual representation
    virtual void UpdateUI() override;
    
    // Override SetupInputBindings to bind additional input actions
    virtual void SetupInputBindings(APlayerController* PlayerController, UInputComponent* PlayerInputComponent) override;
    
    // Override base input handlers
    virtual void OnNextActionTriggered(const FInputActionValue& Value) override;
    virtual void OnPreviousActionTriggered(const FInputActionValue& Value) override;
    virtual void OnConfirmActionTriggered(const FInputActionValue& Value) override;
    
    // Additional input handlers
    UFUNCTION(BlueprintCallable, Category = "Input")
    void OnNextSongActionTriggered(const FInputActionValue& Value);
    
    UFUNCTION(BlueprintCallable, Category = "Input")
    void OnPrevSongActionTriggered(const FInputActionValue& Value);
    
    UFUNCTION(BlueprintCallable, Category = "Input")
    void OnIncreaseDifficultyActionTriggered(const FInputActionValue& Value);
    
    UFUNCTION(BlueprintCallable, Category = "Input")
    void OnDecreaseDifficultyActionTriggered(const FInputActionValue& Value);
    
    UFUNCTION(BlueprintCallable, Category = "Input")
    void OnCycleVisualizerShapeActionTriggered(const FInputActionValue& Value);
    
    UFUNCTION(BlueprintCallable, Category = "Input")
    void OnPlayPreviewActionTriggered(const FInputActionValue& Value);

private:
    // Current song index
    int32 CurrentSongIndex;

    // Update the save file with current selection
    void UpdateSaveData();

    // Get shape name from enum
    FString GetShapeName(EVisualizerShape Shape) const;
};