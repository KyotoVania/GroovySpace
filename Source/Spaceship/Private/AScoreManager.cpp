#include "AScoreManager.h"
#include "UWBP_HUD_Base.h"
#include "Kismet/GameplayStatics.h"
#include "Spaceship/SpaceshipSaveManager.h"
#include "Spaceship/ASpaceshipGameMode.h"

AScoreManager::AScoreManager()
	: CurrentScore(0), ComboCount(0)
{
	PrimaryActorTick.bCanEverTick = false;
}

void AScoreManager::AddScore(int32 Points)
{
	float Multiplier = 1.0f + (FMath::LogX(2.0f, ComboCount + 1) * 0.5f);
	int32 ScoreToAdd = FMath::RoundToInt(Points * Multiplier);
	CurrentScore += ScoreToAdd;

	// Get save manager for high score
	USpaceshipSaveManager* SaveManager = USpaceshipSaveManager::GetSaveManager(GetWorld());
	int32 HighScore = 0;
	if (SaveManager && SaveManager->CurrentSave && SaveManager->CurrentSave->LastSong.Get())
	{
		HighScore = SaveManager->GetBestScore(SaveManager->CurrentSave->LastSong.Get());
	}

	// Update HUD through GameMode
	if (ASpaceshipGameMode* GameMode = Cast<ASpaceshipGameMode>(GetWorld()->GetAuthGameMode()))
	{
		if (UWBP_HUD_Base* HUD = GameMode->GetGameHUD())
		{
			HUD->UpdateScore(CurrentScore, HighScore);
		}
	}
}

void AScoreManager::IncrementCombo()
{
	ComboCount++;
    
	// Update HUD through GameMode
	if (ASpaceshipGameMode* GameMode = Cast<ASpaceshipGameMode>(GetWorld()->GetAuthGameMode()))
	{
		if (UWBP_HUD_Base* HUD = GameMode->GetGameHUD())
		{
			HUD->UpdateCombo(ComboCount);
			HUD->PlayComboFeedback_Implementation();
		}
	}
}

void AScoreManager::BreakCombo()
{
	ComboCount = 0;
	// Update HUD through GameMode
	if (ASpaceshipGameMode* GameMode = Cast<ASpaceshipGameMode>(GetWorld()->GetAuthGameMode()))
	{
		if (UWBP_HUD_Base* HUD = GameMode->GetGameHUD())
		{
			HUD->UpdateCombo(ComboCount);
		}
	}
}