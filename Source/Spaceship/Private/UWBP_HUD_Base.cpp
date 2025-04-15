#include "UWBP_HUD_Base.h"

void UWBP_HUD_Base::PlayComboFeedback_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("Default PlayComboFeedback C++ Implementation"));
}

void UWBP_HUD_Base::UpdateSongName(const FString& SongName)
{
	if (SongNameText)
	{
		// Optionnel: Formater le nom de la chanson (enlever les underscores, etc.)
		FString DisplayName = SongName.Replace(TEXT("_"), TEXT(" "));
		SongNameText->SetText(FText::FromString(DisplayName));
	}
}

void UWBP_HUD_Base::UpdateHealth(float HealthPercentage)
{
	// Mettre à jour la barre de vie
	if (HealthBar)
	{
		HealthBar->SetPercent(HealthPercentage);
	}

	// Mettre à jour le texte des HP (optionnel)
	if (HealthText)
	{
		int32 DisplayHealth = FMath::RoundToInt(HealthPercentage * 100);
		FString HealthString = FString::Printf(TEXT("%d%%"), DisplayHealth);
		HealthText->SetText(FText::FromString(HealthString));
	}
}
