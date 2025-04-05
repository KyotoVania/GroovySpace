#include "BaseSelectorComponent.h"

UBaseSelectorComponent::UBaseSelectorComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SelectionChangedSound = nullptr;
	SelectionConfirmedSound = nullptr;
}

void UBaseSelectorComponent::BeginPlay()
{
	Super::BeginPlay();
    
	// Initialize save manager
	SaveManager = GetSaveManager();
}

USpaceshipSaveManager* UBaseSelectorComponent::GetSaveManager()
{
	if (!SaveManager)
	{
		SaveManager = USpaceshipSaveManager::GetSaveManager(GetWorld());
	}
	return SaveManager;
}

int32 UBaseSelectorComponent::IncrementIndex(int32 CurrentIndex, int32 MaxIndex)
{
	if (MaxIndex <= 0)
	{
		return 0;
	}
    
	return (CurrentIndex + 1) % MaxIndex;
}

int32 UBaseSelectorComponent::DecrementIndex(int32 CurrentIndex, int32 MaxIndex)
{
	if (MaxIndex <= 0)
	{
		return 0;
	}
    
	return (CurrentIndex - 1 + MaxIndex) % MaxIndex;
}

void UBaseSelectorComponent::PlaySelectionChangedSound()
{
	if (SelectionChangedSound)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), SelectionChangedSound);
	}
}

void UBaseSelectorComponent::PlaySelectionConfirmedSound()
{
	if (SelectionConfirmedSound)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), SelectionConfirmedSound);
	}
}