#pragma once

#include "CoreMinimal.h"
#include "BaseSelectorActor.h"
#include "SongOptionsDataAsset.h"
#include "Components/AudioComponent.h"
#include "Components/TextRenderComponent.h"
#include "SongSelectorActor.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSongChanged, USoundWave*, NewSong);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDifficultyChanged, int32, NewDifficulty);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnVisualizerShapeChanged, EVisualizerShape, NewShape);

UCLASS()
class SPACESHIP_API ASongSelectorActor : public ABaseSelectorActor
{
    GENERATED_BODY()

public:
    ASongSelectorActor();

    // Data asset containing all song options
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Songs")
    USongOptionsDataAsset* SongOptions;

    // UI Components
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UTextRenderComponent* SongTitleText;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UTextRenderComponent* DifficultyText;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UTextRenderComponent* HighScoreText;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UTextRenderComponent* VisualizerShapeText;

    // Audio preview component
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UAudioComponent* PreviewComponent;

    // Change event delegates
    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnSongChanged OnSongChanged;

    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnDifficultyChanged OnDifficultyChanged;

    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnVisualizerShapeChanged OnVisualizerShapeChanged;

    // Getter functions
    UFUNCTION(BlueprintCallable, Category = "Songs")
    FSongMetadata GetCurrentSong() const;

    UFUNCTION(BlueprintCallable, Category = "Songs")
    int32 GetCurrentDifficulty() const;

    UFUNCTION(BlueprintCallable, Category = "Songs")
    EVisualizerShape GetCurrentVisualizerShape() const;

    // Selection functions
    UFUNCTION(BlueprintCallable, Category = "Selection")
    void NextSong();

    UFUNCTION(BlueprintCallable, Category = "Selection")
    void PreviousSong();

    UFUNCTION(BlueprintCallable, Category = "Selection")
    void IncreaseDifficulty();

    UFUNCTION(BlueprintCallable, Category = "Selection")
    void DecreaseDifficulty();

    UFUNCTION(BlueprintCallable, Category = "Selection")
    void CycleVisualizerShape();

    // Preview functions
    UFUNCTION(BlueprintCallable, Category = "Preview")
    void TogglePreview();
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    UInputAction* IncreaseDifficultyAction;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    UInputAction* DecreaseDifficultyAction;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    UInputAction* CycleShapeAction;
protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    virtual void UpdateUI() override;

    // Input handling
    virtual void OnNextActionTriggered(const FInputActionValue& Value) override;
    virtual void OnPreviousActionTriggered(const FInputActionValue& Value) override;
    virtual void OnConfirmActionTriggered(const FInputActionValue& Value) override;
    virtual void OnIncreaseDifficultyTriggered(const FInputActionValue& Value);
    virtual void OnDecreaseDifficultyTriggered(const FInputActionValue& Value);
    virtual void OnCycleShapeTriggered(const FInputActionValue& Value);

    // On doit override SetupInputBindings pour ajouter les nouveaux bindings
    virtual void SetupInputBindings(APlayerController* PlayerController, UInputComponent* InputComp) override;

private:
    int32 CurrentSongIndex;
    bool bIsPreviewPlaying;
    FTimerHandle PreviewFadeTimerHandle;

    void StartPreview();
    void StopPreview();
    void UpdateSaveData();
    FString GetShapeName(EVisualizerShape Shape) const;
};