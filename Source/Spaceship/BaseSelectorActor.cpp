#include "BaseSelectorActor.h"
#include "Components/TextRenderComponent.h"
#include "Kismet/GameplayStatics.h"

ABaseSelectorActor::ABaseSelectorActor()
{
	PrimaryActorTick.bCanEverTick = false;
    
	// Create root component
	RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	RootComponent = RootSceneComponent;
    
	// Create title text component
	TitleText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("TitleText"));
	TitleText->SetupAttachment(RootComponent);
	TitleText->SetHorizontalAlignment(EHTA_Center);
	TitleText->SetWorldSize(20.0f);
	TitleText->SetTextRenderColor(FColor::White);
}

void ABaseSelectorActor::BeginPlay()
{
	Super::BeginPlay();
    
	// Initialize save manager
	SaveManager = GetSaveManager();
}

USpaceshipSaveManager* ABaseSelectorActor::GetSaveManager()
{
	if (!SaveManager)
	{
		SaveManager = USpaceshipSaveManager::GetSaveManager(GetWorld());
	}
	return SaveManager;
}

int32 ABaseSelectorActor::IncrementIndex(int32 CurrentIndex, int32 MaxIndex)
{
	if (MaxIndex <= 0)
	{
		return 0;
	}
    
	return (CurrentIndex + 1) % MaxIndex;
}

int32 ABaseSelectorActor::DecrementIndex(int32 CurrentIndex, int32 MaxIndex)
{
	if (MaxIndex <= 0)
	{
		return 0;
	}
    
	return (CurrentIndex - 1 + MaxIndex) % MaxIndex;
}

void ABaseSelectorActor::PlaySelectionChangedSound()
{
	if (SelectionChangedSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, SelectionChangedSound, GetActorLocation());
	}
}

void ABaseSelectorActor::PlaySelectionConfirmedSound()
{
	if (SelectionConfirmedSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, SelectionConfirmedSound, GetActorLocation());
	}
}