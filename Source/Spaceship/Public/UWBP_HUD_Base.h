#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "GameFramework/HUD.h"
#include "UWBP_HUD_Base.generated.h"

UCLASS()
class SPACESHIP_API UWBP_HUD_Base : public UUserWidget
{
	GENERATED_BODY()

public:
	// Existing widgets
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* SongNameText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UProgressBar* HealthBar;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* HealthText;

	// New score and combo widgets
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* CurrentScoreText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* HighScoreText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* ComboText;

	// Animation for feedback
	UPROPERTY(BlueprintReadWrite, Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* ScaleUpFeedback;

	// Update functions
	void UpdateSongName(const FString& SongName);
	void UpdateHealth(float HealthPercentage);
	void UpdateScore(int32 CurrentScore, int32 HighScore);
	void UpdateCombo(int32 ComboCount);
    
	// Existing function
	virtual void PlayComboFeedback_Implementation();
};