#include "UWBP_HUD_Base.h"

void UWBP_HUD_Base::UpdateSongName(const FString& SongName)
{
	UE_LOG(LogTemp, Log, TEXT("Updating song name to: %s"), *SongName);
	if (SongNameText)
	{
		FString DisplayName = SongName.Replace(TEXT("_"), TEXT(" "));
		SongNameText->SetText(FText::FromString(DisplayName));
	}
}

void UWBP_HUD_Base::UpdateHealth(float HealthPercentage)
{
	if (HealthBar)
	{
		HealthBar->SetPercent(HealthPercentage);
	}

	if (HealthText)
	{
		int32 DisplayHealth = FMath::RoundToInt(HealthPercentage * 100);
		FString HealthString = FString::Printf(TEXT("%d%%"), DisplayHealth);
		HealthText->SetText(FText::FromString(HealthString));
	}
}

void UWBP_HUD_Base::UpdateScore(int32 CurrentScore, int32 HighScore)
{
	if (CurrentScoreText)
	{
		CurrentScoreText->SetText(FText::FromString(FString::Printf(TEXT("Score: %d"), CurrentScore)));
	}

	if (HighScoreText)
	{
		HighScoreText->SetText(FText::FromString(FString::Printf(TEXT("High Score: %d"), HighScore)));
	}
}

void UWBP_HUD_Base::UpdateCombo(int32 ComboCount)
{
	if (ComboText)
	{
		ComboText->SetText(FText::FromString(FString::Printf(TEXT("%dx"), ComboCount)));
	}
}

void UWBP_HUD_Base::PlayComboFeedback_Implementation()
{
	if (ScaleUpFeedback)
	{
		PlayAnimation(ScaleUpFeedback);
	}
}