#include "SongSelectorActor.h"

#include "EnhancedInputComponent.h"
#include "Components/TextRenderComponent.h"
#include "Kismet/GameplayStatics.h"

ASongSelectorActor::ASongSelectorActor()
{
    // Initialize variables
    CurrentSongIndex = 0;
    
    // Create song title text
    SongTitleText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("SongTitleText"));
    SongTitleText->SetupAttachment(RootComponent);
    SongTitleText->SetRelativeLocation(FVector(0.0f, 0.0f, 70.0f));
    SongTitleText->SetHorizontalAlignment(EHTA_Center);
    SongTitleText->SetWorldSize(20.0f);
    SongTitleText->SetTextRenderColor(FColor::White);
    
    // Create difficulty text
    DifficultyText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("DifficultyText"));
    DifficultyText->SetupAttachment(RootComponent);
    DifficultyText->SetRelativeLocation(FVector(0.0f, 0.0f, 50.0f));
    DifficultyText->SetHorizontalAlignment(EHTA_Center);
    DifficultyText->SetWorldSize(15.0f);
    DifficultyText->SetTextRenderColor(FColor::Yellow);
    
    // Create high score text
    HighScoreText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("HighScoreText"));
    HighScoreText->SetupAttachment(RootComponent);
    HighScoreText->SetRelativeLocation(FVector(0.0f, 0.0f, 30.0f));
    HighScoreText->SetHorizontalAlignment(EHTA_Center);
    HighScoreText->SetWorldSize(15.0f);
    HighScoreText->SetTextRenderColor(FColor::Green);
    
    // Create visualizer shape text
    VisualizerShapeText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("VisualizerShapeText"));
    VisualizerShapeText->SetupAttachment(RootComponent);
    VisualizerShapeText->SetRelativeLocation(FVector(0.0f, 0.0f, 10.0f));
    VisualizerShapeText->SetHorizontalAlignment(EHTA_Center);
    VisualizerShapeText->SetWorldSize(15.0f);
    VisualizerShapeText->SetTextRenderColor(FColor::Cyan);
    
    // Create audio preview component
    SongPreviewAudio = CreateDefaultSubobject<UAudioComponent>(TEXT("SongPreviewAudio"));
    SongPreviewAudio->SetupAttachment(RootComponent);
    SongPreviewAudio->bAutoActivate = false;
    
    // Set title text
    TitleText->SetText(FText::FromString(TEXT("Song Selector")));
    TitleText->SetRelativeLocation(FVector(0.0f, 0.0f, 100.0f));
}

void ASongSelectorActor::BeginPlay()
{
    Super::BeginPlay();
    
    // Ensure we have at least one song
    if (AvailableSongs.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("No songs defined in SongSelectorActor"));
    }
    
    // Initialize current song index based on last played song in save
    if (SaveManager && SaveManager->CurrentSave && SaveManager->CurrentSave->LastSong)
    {
        // Try to find the last played song in available songs
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
    
    // Update UI with initial values
    UpdateUI();
}

void ASongSelectorActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    // Make sure to stop audio preview when destroying the actor
    StopSongPreview();
    
    Super::EndPlay(EndPlayReason);
}

USoundWave* ASongSelectorActor::GetCurrentSong() const
{
    if (AvailableSongs.Num() == 0)
    {
        return nullptr;
    }
    
    int32 SafeIndex = FMath::Clamp(CurrentSongIndex, 0, AvailableSongs.Num() - 1);
    return AvailableSongs[SafeIndex];
}

int32 ASongSelectorActor::GetCurrentDifficulty() const
{
    if (!SaveManager || !SaveManager->CurrentSave)
    {
        return 5; // Default difficulty
    }
    
    return SaveManager->CurrentSave->Difficulty;
}

EVisualizerShape ASongSelectorActor::GetCurrentVisualizerShape() const
{
    if (!SaveManager || !SaveManager->CurrentSave)
    {
        return EVisualizerShape::Circle; // Default shape
    }
    
    return SaveManager->CurrentSave->VisualizerShape;
}

int32 ASongSelectorActor::GetCurrentSongHighScore() const
{
    USoundWave* CurrentSong = GetCurrentSong();
    if (!CurrentSong || !SaveManager)
    {
        return 0;
    }
    
    return SaveManager->GetBestScore(CurrentSong);
}

void ASongSelectorActor::NextSong()
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
    
    // Update UI
    UpdateUI();
    
    // Restart audio preview if active
    if (SongPreviewAudio && SongPreviewAudio->IsPlaying())
    {
        StartSongPreview();
    }
    
    // Broadcast delegate
    OnSongChanged.Broadcast(GetCurrentSong());
    PlaySelectionChangedSound();
}

void ASongSelectorActor::PreviousSong()
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
    
    // Update UI
    UpdateUI();
    
    // Restart audio preview if active
    if (SongPreviewAudio && SongPreviewAudio->IsPlaying())
    {
        StartSongPreview();
    }
    
    // Broadcast delegate
    OnSongChanged.Broadcast(GetCurrentSong());
    PlaySelectionChangedSound();
}

void ASongSelectorActor::IncreaseDifficulty()
{
    if (!SaveManager || !SaveManager->CurrentSave)
    {
        return;
    }
    
    // Increase difficulty (max 10)
    int32 NewDifficulty = FMath::Clamp(SaveManager->CurrentSave->Difficulty + 1, 1, 10);
    SaveManager->CurrentSave->Difficulty = NewDifficulty;
    UpdateSaveData();
    
    // Update UI
    UpdateUI();
    
    // Broadcast delegate
    OnDifficultyChanged.Broadcast(NewDifficulty);
    PlaySelectionChangedSound();
}

void ASongSelectorActor::DecreaseDifficulty()
{
    if (!SaveManager || !SaveManager->CurrentSave)
    {
        return;
    }
    
    // Decrease difficulty (min 1)
    int32 NewDifficulty = FMath::Clamp(SaveManager->CurrentSave->Difficulty - 1, 1, 10);
    SaveManager->CurrentSave->Difficulty = NewDifficulty;
    UpdateSaveData();
    
    // Update UI
    UpdateUI();
    
    // Broadcast delegate
    OnDifficultyChanged.Broadcast(NewDifficulty);
    PlaySelectionChangedSound();
}

void ASongSelectorActor::CycleVisualizerShape()
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
    
    // Update UI
    UpdateUI();
    
    // Broadcast delegate
    OnVisualizerShapeChanged.Broadcast(SaveManager->CurrentSave->VisualizerShape);
    PlaySelectionChangedSound();
}

void ASongSelectorActor::ConfirmSelection()
{
    if (SaveManager)
    {
        SaveManager->SaveGame();
    }
    
    PlaySelectionConfirmedSound();
}

void ASongSelectorActor::StartSongPreview()
{
    if (!SongPreviewAudio)
    {
        return;
    }
    
    USoundWave* CurrentSong = GetCurrentSong();
    if (!CurrentSong)
    {
        return;
    }
    
    // Stop current playback
    SongPreviewAudio->Stop();
    
    // Update sound and settings
    SongPreviewAudio->SetSound(CurrentSong);
    SongPreviewAudio->SetVolumeMultiplier(0.3f); // Lower volume for preview
    SongPreviewAudio->SetPitchMultiplier(1.0f);
    
    // Start playback a bit into the song to skip intros
    float StartTime = FMath::Min(5.0f, CurrentSong->Duration * 0.2f);
    SongPreviewAudio->Play(StartTime);
}

void ASongSelectorActor::StopSongPreview()
{
    if (SongPreviewAudio && SongPreviewAudio->IsPlaying())
    {
        SongPreviewAudio->Stop();
    }
}

void ASongSelectorActor::UpdateUI()
{
    USoundWave* CurrentSong = GetCurrentSong();
    
    // Update song title
    if (SongTitleText && CurrentSong)
    {
        SongTitleText->SetText(FText::FromString(CurrentSong->GetName()));
    }
    else if (SongTitleText)
    {
        SongTitleText->SetText(FText::FromString(TEXT("No Songs Available")));
    }
    
    // Update difficulty text
    if (DifficultyText)
    {
        int32 Difficulty = GetCurrentDifficulty();
        DifficultyText->SetText(FText::Format(
            FText::FromString(TEXT("Difficulty: {0}/10")), 
            FText::AsNumber(Difficulty)
        ));
    }
    
    // Update high score text
    if (HighScoreText)
    {
        int32 HighScore = GetCurrentSongHighScore();
        HighScoreText->SetText(FText::Format(
            FText::FromString(TEXT("High Score: {0}")), 
            FText::AsNumber(HighScore)
        ));
    }
    
    // Update visualizer shape text
    if (VisualizerShapeText)
    {
        EVisualizerShape Shape = GetCurrentVisualizerShape();
        VisualizerShapeText->SetText(FText::Format(
            FText::FromString(TEXT("Visualizer: {0}")), 
            FText::FromString(GetShapeName(Shape))
        ));
    }
}

void ASongSelectorActor::UpdateSaveData()
{
    if (SaveManager)
    {
        SaveManager->SaveGame();
    }
}

FString ASongSelectorActor::GetShapeName(EVisualizerShape Shape) const
{
    switch (Shape)
    {
        case EVisualizerShape::Aligned:
            return TEXT("Aligned");
        case EVisualizerShape::Circle:
            return TEXT("Circle");
        case EVisualizerShape::HalfCircle:
            return TEXT("Half Circle");
        case EVisualizerShape::CustomCircle:
            return TEXT("Custom Circle");
        default:
            return TEXT("Unknown");
    }
}

void ASongSelectorActor::SetupInputBindings(APlayerController* PlayerController, UInputComponent* PlayerInputComponent)
{
    // First call the base class implementation to set up the basic inputs
    Super::SetupInputBindings(PlayerController, PlayerInputComponent);
    
    // Bind additional input actions
    UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
    if (!EnhancedInputComponent)
    {
        return;
    }
    
    if (NextSongAction)
    {
        EnhancedInputComponent->BindAction(NextSongAction, ETriggerEvent::Triggered, this, &ASongSelectorActor::OnNextSongActionTriggered);
    }
    
    if (PrevSongAction)
    {
        EnhancedInputComponent->BindAction(PrevSongAction, ETriggerEvent::Triggered, this, &ASongSelectorActor::OnPrevSongActionTriggered);
    }
    
    if (IncreaseDifficultyAction)
    {
        EnhancedInputComponent->BindAction(IncreaseDifficultyAction, ETriggerEvent::Triggered, this, &ASongSelectorActor::OnIncreaseDifficultyActionTriggered);
    }
    
    if (DecreaseDifficultyAction)
    {
        EnhancedInputComponent->BindAction(DecreaseDifficultyAction, ETriggerEvent::Triggered, this, &ASongSelectorActor::OnDecreaseDifficultyActionTriggered);
    }
    
    if (CycleVisualizerShapeAction)
    {
        EnhancedInputComponent->BindAction(CycleVisualizerShapeAction, ETriggerEvent::Triggered, this, &ASongSelectorActor::OnCycleVisualizerShapeActionTriggered);
    }
    
    if (PlayPreviewAction)
    {
        EnhancedInputComponent->BindAction(PlayPreviewAction, ETriggerEvent::Triggered, this, &ASongSelectorActor::OnPlayPreviewActionTriggered);
    }
}

void ASongSelectorActor::OnNextActionTriggered(const FInputActionValue& Value)
{
    // Default next action (can be overridden by more specific bindings)
    NextSong();
}

void ASongSelectorActor::OnPreviousActionTriggered(const FInputActionValue& Value)
{
    // Default previous action (can be overridden by more specific bindings)
    PreviousSong();
}

void ASongSelectorActor::OnConfirmActionTriggered(const FInputActionValue& Value)
{
    ConfirmSelection();
}

void ASongSelectorActor::OnNextSongActionTriggered(const FInputActionValue& Value)
{
    NextSong();
}

void ASongSelectorActor::OnPrevSongActionTriggered(const FInputActionValue& Value)
{
    PreviousSong();
}

void ASongSelectorActor::OnIncreaseDifficultyActionTriggered(const FInputActionValue& Value)
{
    IncreaseDifficulty();
}

void ASongSelectorActor::OnDecreaseDifficultyActionTriggered(const FInputActionValue& Value)
{
    DecreaseDifficulty();
}

void ASongSelectorActor::OnCycleVisualizerShapeActionTriggered(const FInputActionValue& Value)
{
    CycleVisualizerShape();
}

void ASongSelectorActor::OnPlayPreviewActionTriggered(const FInputActionValue& Value)
{
    if (SongPreviewAudio && SongPreviewAudio->IsPlaying())
    {
        StopSongPreview();
    }
    else
    {
        StartSongPreview();
    }
}