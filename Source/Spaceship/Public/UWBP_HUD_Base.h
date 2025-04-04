#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UWBP_HUD_Base.generated.h"

UCLASS()
class SPACESHIP_API UWBP_HUD_Base : public UUserWidget
{
	GENERATED_BODY()

public:
	// ✅ Exposer la fonction pour qu'elle puisse être appelée en Blueprint
	UFUNCTION(BlueprintNativeEvent, Category = "HUD")
	void PlayComboFeedback();
	virtual void PlayComboFeedback_Implementation();
};
