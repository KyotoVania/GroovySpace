#include "UWBP_GameOver.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/ScrollBox.h"
#include "Kismet/GameplayStatics.h"
#include "Spaceship/SpaceshipSaveManager.h"
#include "HighScoreUtils.h"
#include "Spaceship/SpaceshipSaveManager.h"

void UWBP_GameOver::NativeConstruct()
{
    Super::NativeConstruct();

    if (RetryButton)
    {
        RetryButton->OnClicked.AddDynamic(this, &UWBP_GameOver::OnRetryClicked);
    }

    if (ReturnButton)
    {
        ReturnButton->OnClicked.AddDynamic(this, &UWBP_GameOver::OnReturnClicked);
    }
}

void UWBP_GameOver::InitializeGameOver(bool bWon, const FString& SongName, int32 Score)
{
    // Met à jour les textes de base
    if (StatusText)
    {
        StatusText->SetText(FText::FromString(bWon ? TEXT("Victory!") : TEXT("Game Over")));
    }

    if (SongNameText)
    {
        SongNameText->SetText(FText::FromString(FString::Printf(TEXT("Song: %s"), *SongName)));
    }

    if (ScoreText)
    {
        ScoreText->SetText(FText::FromString(FString::Printf(TEXT("Score: %d"), Score)));
    }

    // Récupérer le SaveManager et le CurrentSoundWave
    USpaceshipSaveManager* SaveManager = USpaceshipSaveManager::GetSaveManager(GetWorld());
    if (SaveManager && SaveManager->CurrentSave && SaveManager->CurrentSave->LastSong.Get())
    {
        USoundWave* CurrentSong = SaveManager->CurrentSave->LastSong.Get();
        UpdateHighScoresDisplay(CurrentSong, Score);
    }
}

void UWBP_GameOver::UpdateHighScoresDisplay(USoundWave* SoundWave, int32 CurrentScore)
{
    if (!HighScoresScrollBox || !SoundWave) return;

    USpaceshipSaveManager* SaveManager = USpaceshipSaveManager::GetSaveManager(GetWorld());
    if (!SaveManager || !SaveManager->CurrentSave) return;

    // Vider le ScrollBox existant
    HighScoresScrollBox->ClearChildren();

    // Obtenir le format complet des high scores
    FString HighScores = SaveManager->GetFormattedHighScores(SoundWave);
    
    // Vérifier si c'est un nouveau high score
    int32 BestScore = SaveManager->GetBestScore(SoundWave);
    bool bIsNewHighScore = CurrentScore > BestScore;

    // Mettre à jour l'indicateur de nouveau high score
    if (NewHighScoreText)
    {
        NewHighScoreText->SetVisibility(
            bIsNewHighScore ? ESlateVisibility::Visible : ESlateVisibility::Collapsed
        );
    }

    // Créer et ajouter chaque ligne de score
    TArray<FString> ScoreLines;
    HighScores.ParseIntoArrayLines(ScoreLines);
    
    for (const FString& ScoreLine : ScoreLines)
    {
        // Utiliser un nom différent pour la variable locale
        UTextBlock* ScoreLineText = NewObject<UTextBlock>(HighScoresScrollBox);
        ScoreLineText->SetText(FText::FromString(ScoreLine));
        
        // Mettre en évidence le score actuel
        if (ScoreLine.Contains(FString::FromInt(CurrentScore)))
        {
            ScoreLineText->SetColorAndOpacity(FSlateColor(FLinearColor(1.0f, 0.8f, 0.0f, 1.0f)));
        }
        
        HighScoresScrollBox->AddChild(ScoreLineText);
    }
}

void UWBP_GameOver::OnRetryClicked()
{
    // Restore game input mode before reloading level
    if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
    {
        FInputModeGameOnly InputMode;
        PC->SetInputMode(InputMode);
        PC->SetShowMouseCursor(false);
    }

    // Reload current level
    UGameplayStatics::OpenLevel(this, FName(*UGameplayStatics::GetCurrentLevelName(this)));
}

void UWBP_GameOver::OnReturnClicked()
{
    // Restore game input mode before returning to menu
    if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
    {
        FInputModeGameOnly InputMode;
        PC->SetInputMode(InputMode);
        PC->SetShowMouseCursor(false);
    }

    // Return to main menu level
    UGameplayStatics::OpenLevel(this, FName("MainMenu"));
}