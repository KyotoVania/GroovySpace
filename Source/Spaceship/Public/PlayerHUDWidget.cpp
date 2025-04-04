// HUDManagerComponent.cpp
#include "PlayerHUDWidget.h"

UHUDManagerComponent::UHUDManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	CurrentHealth = 100.0f;
	CurrentScore = 0;
}

void UHUDManagerComponent::BeginPlay()
{
	Super::BeginPlay();
	// Initialiser les valeurs ici si nécessaire
}

void UHUDManagerComponent::UpdateHealth(float NewHealth)
{
	CurrentHealth = NewHealth;
	OnHealthUpdated.Broadcast(CurrentHealth);
}

void UHUDManagerComponent::UpdateScore(int32 NewScore)
{
	CurrentScore = NewScore;
	OnScoreUpdated.Broadcast(CurrentScore);
}