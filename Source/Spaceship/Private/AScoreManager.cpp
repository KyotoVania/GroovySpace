#include "AScoreManager.h"
#include "UWBP_HUD_Base.h"
#include "Kismet/GameplayStatics.h"
#include "Spaceship/SpaceshipSaveManager.h"

AScoreManager::AScoreManager()
	: CurrentScore(0), ComboCount(0), Character(nullptr) // Initialize members
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

	// Update HUD
	Character = Cast<ASpaceshipCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	if (Character && Character->GetHUDWidget())
	{
		Character->GetHUDWidget()->UpdateScore(CurrentScore, HighScore);
	}
}

void AScoreManager::IncrementCombo()
{
	ComboCount++;
    
	if (Character && Character->GetHUDWidget())
	{
		Character->GetHUDWidget()->UpdateCombo(ComboCount);
		Character->GetHUDWidget()->PlayComboFeedback_Implementation();
	}
}

void AScoreManager::BreakCombo()
{
	ComboCount = 0;
	if (Character && Character->GetHUDWidget())
	{
		Character->GetHUDWidget()->UpdateCombo(ComboCount);
	}
}