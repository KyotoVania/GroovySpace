#include "AScoreManager.h"
#include "UWBP_HUD_Base.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "TimerManager.h" // Ajout pour gérer le cooldown

AScoreManager::AScoreManager()
{
	PrimaryActorTick.bCanEverTick = false;
	CurrentHealth = MaxHealth;
	bIsInvincible = false;
	InvincibilityDuration = 1.0f; // 1 seconde d'invincibilité après un hit
}

void AScoreManager::AddScore(int32 Points)
{
	float Multiplier = 1.0f + (FMath::LogX(2.0f, ComboCount + 1) * 0.5f);
	int32 ScoreToAdd = FMath::RoundToInt(Points * Multiplier);

	CurrentScore += ScoreToAdd;

	UE_LOG(LogTemp, Log, TEXT("Score updated: %d (Combo: %d, Multiplier: %.2f)"), CurrentScore, ComboCount, Multiplier);
}

void AScoreManager::IncrementCombo()
{
	ComboCount++;

	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (!PC) return;

	TArray<UUserWidget*> FoundWidgets;
	UWidgetBlueprintLibrary::GetAllWidgetsOfClass(GetWorld(), FoundWidgets, UWBP_HUD_Base::StaticClass(), false);
	if (FoundWidgets.Num() > 0)
	{
		UWBP_HUD_Base* HUD = Cast<UWBP_HUD_Base>(FoundWidgets[0]);
		if (HUD)
		{
			HUD->PlayComboFeedback();
			UE_LOG(LogTemp, Warning, TEXT("Combo animation triggered!"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to find HUD!"));
	}
}

void AScoreManager::BreakCombo()
{
	ComboCount = 0;
}

void AScoreManager::UpdateHealth(float Health)
{
	CurrentHealth = Health;
}


void AScoreManager::ResetInvincibility()
{
	bIsInvincible = false;
	UE_LOG(LogTemp, Log, TEXT("Invincibility ended."));
}
