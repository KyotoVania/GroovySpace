#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/VerticalBox.h"
#include "Components/ScrollBox.h"
#include "UWBP_GameOver.generated.h"

UCLASS()
class SPACESHIP_API UWBP_GameOver : public UUserWidget
{
	GENERATED_BODY()

public:
	// Widgets existants
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* StatusText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* SongNameText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* ScoreText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UButton* RetryButton;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UButton* ReturnButton;

	// Nouveau widget pour les high scores
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UScrollBox* HighScoresScrollBox;

	// Nouveau widget pour afficher si c'est un nouveau high score
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* NewHighScoreText;

	// Initialize widget avec game results
	UFUNCTION(BlueprintCallable, Category = "Game Over")
	void InitializeGameOver(bool bWon, const FString& SongName, int32 Score);

protected:
	virtual void NativeConstruct() override;

private:
	UFUNCTION()
	void OnRetryClicked();

	UFUNCTION() 
	void OnReturnClicked();

	// Nouvelle méthode pour mettre à jour l'affichage des high scores
	void UpdateHighScoresDisplay(class USoundWave* SoundWave, int32 CurrentScore);
};