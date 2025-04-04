// HUDManagerComponent.h
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AScoreManager.h"
#include "PlayerHUDWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHealthUpdated, float, NewHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnScoreUpdated, int32, NewScore);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SPACESHIP_API UHUDManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UHUDManagerComponent();

	UPROPERTY(BlueprintAssignable, Category = "HUD")
	FOnHealthUpdated OnHealthUpdated;

	UPROPERTY(BlueprintAssignable, Category = "HUD")
	FOnScoreUpdated OnScoreUpdated;

	UFUNCTION(BlueprintCallable, Category = "HUD")
	void UpdateHealth(float NewHealth);

	UFUNCTION(BlueprintCallable, Category = "HUD")
	void UpdateScore(int32 NewScore);

	UFUNCTION(BlueprintCallable, Category = "HUD")
	void SetScoreManager(AScoreManager* Manager) { ScoreManager = Manager; }
	
	UPROPERTY()
	AScoreManager* ScoreManager;
protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	float CurrentHealth;

	UPROPERTY()
	int32 CurrentScore;

};