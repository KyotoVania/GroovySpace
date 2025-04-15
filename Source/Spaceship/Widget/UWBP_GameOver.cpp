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
    if (!SaveManager) return;

    // Vide le ScrollBox existant
    HighScoresScrollBox->ClearChildren();

    // Récupère les high scores existants
    const FSongScores* SongScores = SaveManager->CurrentSave->SongHighScores.SongScores.Find(SoundWave);
    if (!SongScores) return;

    // Vérifie si c'est un nouveau high score
    bool bIsNewHighScore = false;
    for (int32 i = 0; i < SongScores->TopScores.Num(); i++)
    {
        if (CurrentScore > SongScores->TopScores[i].Score)
        {
            bIsNewHighScore = true;
            break;
        }
    }

    // Met à jour le texte de nouveau high score si nécessaire
    if (NewHighScoreText)
    {
        NewHighScoreText->SetVisibility(bIsNewHighScore ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
    }

    // Crée une entrée pour chaque score
    for (int32 i = 0; i < SongScores->TopScores.Num(); i++)
    {
        const FScoreEntry& ScoreEntry = SongScores->TopScores[i];
        
        ScoreText = NewObject<UTextBlock>(HighScoresScrollBox);
        FString ScoreString = FString::Printf(TEXT("%d. %d pts (Diff: %d) - %s"),
            i + 1,
            ScoreEntry.Score,
            ScoreEntry.Difficulty,
            *ScoreEntry.DateAchieved);

        // Applique un style spécial si c'est le score actuel
        if (ScoreEntry.Score == CurrentScore)
        {
            ScoreString = FString::Printf(TEXT("> %s <"), *ScoreString);
        }

        ScoreText->SetText(FText::FromString(ScoreString));
        
        // Optionnel: Applique un style différent pour le score actuel
        if (ScoreEntry.Score == CurrentScore)
        {
            FSlateColor HighlightColor = FSlateColor(FLinearColor(1.0f, 0.8f, 0.0f, 1.0f)); // Gold color
            ScoreText->SetColorAndOpacity(HighlightColor);
        }

        HighScoresScrollBox->AddChild(ScoreText);
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