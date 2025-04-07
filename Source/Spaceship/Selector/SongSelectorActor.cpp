#include "SongSelectorActor.h"

#include "EnhancedInputComponent.h"
#include "Components/TextRenderComponent.h"
#include "Kismet/GameplayStatics.h"

ASongSelectorActor::ASongSelectorActor()
{
    // Create UI components
    SongTitleText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("SongTitleText"));
    SongTitleText->SetupAttachment(RootComponent);
    SongTitleText->SetRelativeLocation(FVector(0.0f, 0.0f, 70.0f));
    SongTitleText->SetHorizontalAlignment(EHTA_Center);
    SongTitleText->SetWorldSize(20.0f);
    SongTitleText->SetTextRenderColor(FColor::White);
    
    DifficultyText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("DifficultyText"));
    DifficultyText->SetupAttachment(RootComponent);
    DifficultyText->SetRelativeLocation(FVector(0.0f, 0.0f, 50.0f));
    DifficultyText->SetHorizontalAlignment(EHTA_Center);
    DifficultyText->SetWorldSize(15.0f);
    DifficultyText->SetTextRenderColor(FColor::Yellow);
    
    HighScoreText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("HighScoreText"));
    HighScoreText->SetupAttachment(RootComponent);
    HighScoreText->SetRelativeLocation(FVector(0.0f, 0.0f, 30.0f));
    HighScoreText->SetHorizontalAlignment(EHTA_Center);
    HighScoreText->SetWorldSize(15.0f);
    HighScoreText->SetTextRenderColor(FColor::Green);
    
    VisualizerShapeText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("VisualizerShapeText"));
    VisualizerShapeText->SetupAttachment(RootComponent);
    VisualizerShapeText->SetRelativeLocation(FVector(0.0f, 0.0f, 10.0f));
    VisualizerShapeText->SetHorizontalAlignment(EHTA_Center);
    VisualizerShapeText->SetWorldSize(15.0f);
    VisualizerShapeText->SetTextRenderColor(FColor::Cyan);
    
    PreviewComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("PreviewComponent"));
    PreviewComponent->SetupAttachment(RootComponent);
    PreviewComponent->bAutoActivate = false;
    
    TitleText->SetText(FText::FromString(TEXT("Song Selector")));
    TitleText->SetRelativeLocation(FVector(0.0f, 0.0f, 100.0f));
    
    CurrentSongIndex = 0;
    bIsPreviewPlaying = false;
}

void ASongSelectorActor::BeginPlay()
{
    Super::BeginPlay();
    
    if (!SongOptions || SongOptions->AvailableSongs.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("SongSelectorActor: No songs available in SongOptions"));
        return;
    }
    
    if (SaveManager && SaveManager->CurrentSave && !SaveManager->CurrentSave->LastSong.IsNull()) // Vérifie si le SoftPtr est défini
    {
        // Résout le SoftPtr pour obtenir le chemin de l'asset
        FSoftObjectPath LastSongPath = SaveManager->CurrentSave->LastSong.ToSoftObjectPath();

        bool bFound = false;
        for (int32 i = 0; i < SongOptions->AvailableSongs.Num(); ++i)
        {
            if (SongOptions->AvailableSongs[i].SoundWave) // Vérifie si le SoundWave dans le DataAsset est valide
            {
                // Compare les chemins des assets
                FSoftObjectPath CurrentSongPath(SongOptions->AvailableSongs[i].SoundWave);
                if (CurrentSongPath == LastSongPath)
                {
                    CurrentSongIndex = i;
                    bFound = true;
                    UE_LOG(LogTemp, Log, TEXT("Dernière chanson '%s' trouvée à l'index %d"), *LastSongPath.GetAssetName(), CurrentSongIndex);
                    break;
                }
            }
        }
        if (!bFound)
        {
            UE_LOG(LogTemp, Warning, TEXT("Dernière chanson sauvegardée ('%s') non trouvée dans SongOptions. Retour à l'index 0."), *LastSongPath.GetAssetName());
            CurrentSongIndex = 0; // Reset si non trouvé
            // Optionnel: Mettre à jour la sauvegarde pour éviter de chercher la prochaine fois
            // SaveManager->CurrentSave->LastSong = SongOptions->AvailableSongs.IsValidIndex(0) ? SongOptions->AvailableSongs[0].SoundWave : nullptr;
            // UpdateSaveData();
        }
    }
    else
    {
        CurrentSongIndex = 0; // Pas de sauvegarde ou LastSong non défini
    }

    // Set up input bindings
    APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (PlayerController)
    {
        SetupInputBindings(PlayerController, PlayerController->InputComponent);
        UE_LOG(LogTemp, Warning, TEXT("Input bindings set up for songSelector"));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to get PlayerController in SkinSelectorActor"));
    }
    
    UpdateUI();
}

void ASongSelectorActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    StopPreview();
    Super::EndPlay(EndPlayReason);
}

FSongMetadata ASongSelectorActor::GetCurrentSong() const
{
    if (!SongOptions || SongOptions->AvailableSongs.Num() == 0)
    {
        return FSongMetadata();
    }
    
    return SongOptions->AvailableSongs[CurrentSongIndex];
}

int32 ASongSelectorActor::GetCurrentDifficulty() const
{
    return SaveManager ? SaveManager->CurrentSave->Difficulty : 5;
}

EVisualizerShape ASongSelectorActor::GetCurrentVisualizerShape() const
{
    return SaveManager ? SaveManager->CurrentSave->VisualizerShape : EVisualizerShape::Circle;
}

void ASongSelectorActor::NextSong()
{
    UE_LOG(LogTemp, Log, TEXT("Next song selected"));
    if (!SongOptions || SongOptions->AvailableSongs.Num() == 0) return;
    
    CurrentSongIndex = (CurrentSongIndex + 1) % SongOptions->AvailableSongs.Num();
    
    if (SaveManager && SaveManager->CurrentSave)
    {
        SaveManager->CurrentSave->LastSong = GetCurrentSong().SoundWave;
        UpdateSaveData();
    }
    UE_LOG(LogTemp, Log, TEXT("Current song index: %d"), CurrentSongIndex);
    UpdateUI();
    if (bIsPreviewPlaying) StartPreview();
    OnSongChanged.Broadcast(GetCurrentSong().SoundWave);
    PlaySelectionChangedSound();
}

void ASongSelectorActor::PreviousSong()
{
    if (!SongOptions || SongOptions->AvailableSongs.Num() == 0) return;
    
    CurrentSongIndex = (CurrentSongIndex - 1 + SongOptions->AvailableSongs.Num()) % SongOptions->AvailableSongs.Num();
    
    if (SaveManager && SaveManager->CurrentSave)
    {
        SaveManager->CurrentSave->LastSong = GetCurrentSong().SoundWave;
        UpdateSaveData();
    }
    
    UpdateUI();
    if (bIsPreviewPlaying) StartPreview();
    OnSongChanged.Broadcast(GetCurrentSong().SoundWave);
    PlaySelectionChangedSound();
}

void ASongSelectorActor::IncreaseDifficulty()
{
    if (!SaveManager || !SaveManager->CurrentSave) return;
    
    SaveManager->CurrentSave->Difficulty = FMath::Clamp(SaveManager->CurrentSave->Difficulty + 1, 1, 10);
    UpdateSaveData();
    UpdateUI();
    OnDifficultyChanged.Broadcast(SaveManager->CurrentSave->Difficulty);
    PlaySelectionChangedSound();
}

void ASongSelectorActor::DecreaseDifficulty()
{
    if (!SaveManager || !SaveManager->CurrentSave) return;
    
    SaveManager->CurrentSave->Difficulty = FMath::Clamp(SaveManager->CurrentSave->Difficulty - 1, 1, 10);
    UpdateSaveData();
    UpdateUI();
    OnDifficultyChanged.Broadcast(SaveManager->CurrentSave->Difficulty);
    PlaySelectionChangedSound();
}

void ASongSelectorActor::CycleVisualizerShape()
{
    if (!SaveManager || !SaveManager->CurrentSave) return;
    
    int32 CurrentShape = static_cast<int32>(SaveManager->CurrentSave->VisualizerShape);
    int32 NextShape = (CurrentShape + 1) % static_cast<int32>(EVisualizerShape::Max);
    SaveManager->CurrentSave->VisualizerShape = static_cast<EVisualizerShape>(NextShape);
    
    UpdateSaveData();
    UpdateUI();
    OnVisualizerShapeChanged.Broadcast(SaveManager->CurrentSave->VisualizerShape);
    PlaySelectionChangedSound();
}

void ASongSelectorActor::UpdateUI()
{
    FSongMetadata CurrentSong = GetCurrentSong();
    
    if (SongTitleText)
    {
        FString DisplayName = CurrentSong.DisplayName.IsEmpty() ? 
            FString::Printf(TEXT("Song %d"), CurrentSongIndex + 1) : CurrentSong.DisplayName;
        SongTitleText->SetText(FText::FromString(DisplayName));
    }
    
    if (DifficultyText)
    {
        int32 Difficulty = GetCurrentDifficulty();
        DifficultyText->SetText(FText::Format(
            FText::FromString(TEXT("Difficulty: {0}/10")),
            FText::AsNumber(Difficulty)));
    }
    
    if (HighScoreText && SaveManager)
    {
        int32 HighScore = SaveManager->GetBestScore(CurrentSong.SoundWave);
        HighScoreText->SetText(FText::Format(
            FText::FromString(TEXT("High Score: {0}")),
            FText::AsNumber(HighScore)));
    }
    
    if (VisualizerShapeText)
    {
        EVisualizerShape Shape = GetCurrentVisualizerShape();
        VisualizerShapeText->SetText(FText::Format(
            FText::FromString(TEXT("Shape: {0}")),
            FText::FromString(GetShapeName(Shape))));
    }
}

void ASongSelectorActor::TogglePreview()
{
    if (bIsPreviewPlaying)
    {
        StopPreview();
    }
    else
    {
        StartPreview();
    }
}

void ASongSelectorActor::StartPreview()
{
    if (!PreviewComponent) return;
    
    FSongMetadata CurrentSong = GetCurrentSong();
    if (!CurrentSong.SoundWave) return;
    
    PreviewComponent->SetSound(CurrentSong.SoundWave);
    PreviewComponent->SetVolumeMultiplier(0.5f);
    PreviewComponent->Play(CurrentSong.PreviewStartTime);
    bIsPreviewPlaying = true;
}

void ASongSelectorActor::StopPreview()
{
    if (!PreviewComponent) return;
    
    PreviewComponent->Stop();
    bIsPreviewPlaying = false;
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
            return TEXT("Custom");
        default:
            return TEXT("Unknown");
    }
}

// Modifie les handlers existants pour appeler Super:: et assigne Confirm à TogglePreview
void ASongSelectorActor::OnNextActionTriggered(const FInputActionValue& Value)
{
    if (!bPlayerIsInside)
    {
        UE_LOG(LogTemp, Verbose, TEXT("Input 'Confirm' ignoré: Joueur hors zone pour %s"), *GetName());
        return; // Ne rien faire si le joueur n'est pas dans la box
    }

    // --- Reste du code de la fonction ---
    float CurrentTime = GetWorld()->GetTimeSeconds();
    if (CurrentTime - LastInputTime < InputCooldown)
    {
        return; // Ignore input if cooldown is active
    }
    LastInputTime = CurrentTime;
    NextSong();
}

void ASongSelectorActor::OnPreviousActionTriggered(const FInputActionValue& Value)
{
    if (!bPlayerIsInside)
        return;

    float CurrentTime = GetWorld()->GetTimeSeconds();
    if (CurrentTime - LastInputTime < InputCooldown)
        return;
    
    LastInputTime = CurrentTime;
    PreviousSong();
}

void ASongSelectorActor::OnConfirmActionTriggered(const FInputActionValue& Value)
{
    Super::OnConfirmActionTriggered(Value); // <<< APPEL IMPORTANT POUR COOLDOWN/CHECK ZONE
     if (!bPlayerIsInside && GetWorld()->GetTimeSeconds() - LastInputTime < InputCooldown) return;

    // Décide ce que fait Confirm ici, par exemple :
    TogglePreview();
    PlaySelectionConfirmedSound(); // Le son de confirmation de la base class

    // Note: Le Super::OnConfirmActionTriggered de BaseSelectorActor ne fait rien par défaut,
    // mais l'appeler est une bonne pratique si la classe de base évolue.
    // La fonction ConfirmSelection() a été déplacée ici ou renommée TogglePreview.
}

// Ajoute les nouveaux handlers
void ASongSelectorActor::OnIncreaseDifficultyTriggered(const FInputActionValue& Value)
{
    if (!bPlayerIsInside)
        return;

    float CurrentTime = GetWorld()->GetTimeSeconds();
    if (CurrentTime - LastInputTime < InputCooldown)
        return;
    
    LastInputTime = CurrentTime;
    IncreaseDifficulty();
     UE_LOG(LogTemp, Log, TEXT("Input: Increase Difficulty"));
}

void ASongSelectorActor::OnDecreaseDifficultyTriggered(const FInputActionValue& Value)
{
    if (!bPlayerIsInside)
        return;

    float CurrentTime = GetWorld()->GetTimeSeconds();
    if (CurrentTime - LastInputTime < InputCooldown)
        return;
    
    LastInputTime = CurrentTime;
    DecreaseDifficulty();
     UE_LOG(LogTemp, Log, TEXT("Input: Decrease Difficulty"));
}

void ASongSelectorActor::OnCycleShapeTriggered(const FInputActionValue& Value)
{
    // Peut utiliser un autre cooldown si besoin, ou le même
    if (!bPlayerIsInside)
        return;

    float CurrentTime = GetWorld()->GetTimeSeconds();
    if (CurrentTime - LastInputTime < InputCooldown)
        return;
    
    LastInputTime = CurrentTime;
    CycleVisualizerShape();
    UE_LOG(LogTemp, Log, TEXT("Input: Cycle Shape"));
}

void ASongSelectorActor::SetupInputBindings(APlayerController* PlayerController, UInputComponent* InputComp)
{
    // Appelle d'abord la fonction de base pour lier Next/Previous/Confirm
    Super::SetupInputBindings(PlayerController, InputComp);

    // Lie les nouvelles actions spécifiques à ce sélecteur
    if (UEnhancedInputComponent* EnhancedInputComp = Cast<UEnhancedInputComponent>(InputComp))
    {
        if (IncreaseDifficultyAction)
        {
            EnhancedInputComp->BindAction(IncreaseDifficultyAction, ETriggerEvent::Triggered, this, &ASongSelectorActor::OnIncreaseDifficultyTriggered);
        }
        if (DecreaseDifficultyAction)
        {
            EnhancedInputComp->BindAction(DecreaseDifficultyAction, ETriggerEvent::Triggered, this, &ASongSelectorActor::OnDecreaseDifficultyTriggered);
        }
        if (CycleShapeAction)
        {
            EnhancedInputComp->BindAction(CycleShapeAction, ETriggerEvent::Triggered, this, &ASongSelectorActor::OnCycleShapeTriggered);
        }
        UE_LOG(LogTemp, Log, TEXT("Bindings spécifiques à SongSelectorActor ajoutés."));
    }
}