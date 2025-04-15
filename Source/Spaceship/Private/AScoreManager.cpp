#include "AScoreManager.h"
#include "UWBP_HUD_Base.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "TimerManager.h" // Ajout pour gérer le cooldown
#include "Spaceship/SpaceshipCharacter.h"

AScoreManager::AScoreManager()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AScoreManager::AddScore(int32 Points)
{
	float Multiplier = 1.0f + (FMath::LogX(2.0f, ComboCount + 1) * 0.5f);
	int32 ScoreToAdd = FMath::RoundToInt(Points * Multiplier);

	CurrentScore += ScoreToAdd;
	Character = Cast<ASpaceshipCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));

	UE_LOG(LogTemp, Log, TEXT("Score updated: %d (Combo: %d, Multiplier: %.2f)"), CurrentScore, ComboCount, Multiplier);
}

void AScoreManager::IncrementCombo()
{
	ComboCount++;

	if (Character && Character->GetHUDWidget())
	{
		Character->GetHUDWidget()->PlayComboFeedback_Implementation();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to find HUD!"));
		if (!Character)
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to find Character!"));
		}
		if (!Character->GetHUDWidget())
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to find HUD Widget!"));
		}
	}
}

void AScoreManager::BreakCombo()
{
	ComboCount = 0;
}
