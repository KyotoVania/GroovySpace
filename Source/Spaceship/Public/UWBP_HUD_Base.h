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
	// Widgets pour l'affichage
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* SongNameText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UProgressBar* HealthBar;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* HealthText;

	// Fonctions de mise à jour
	UFUNCTION()
	void UpdateSongName(const FString& SongName);

	UFUNCTION()
	void UpdateHealth(float HealthPercentage);

	// Fonction existante
	virtual void PlayComboFeedback_Implementation();
};