#include "BaseSelectorActor.h"
#include "Components/TextRenderComponent.h"
#include "Kismet/GameplayStatics.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

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

void ABaseSelectorActor::SetupInputBindings(APlayerController* PlayerController, UInputComponent* PlayerInputComponent)
{
	if (!PlayerController || !PlayerInputComponent)
	{
		return;
	}
    
	// Get the local player enhanced input subsystem
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
	if (!Subsystem)
	{
		return;
	}
    
	// Get the enhanced input component
	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (!EnhancedInputComponent)
	{
		return;
	}
    
	// Bind the actions
	if (SelectNextAction)
	{
		EnhancedInputComponent->BindAction(SelectNextAction, ETriggerEvent::Triggered, this, &ABaseSelectorActor::OnNextActionTriggered);
	}
    
	if (SelectPreviousAction)
	{
		EnhancedInputComponent->BindAction(SelectPreviousAction, ETriggerEvent::Triggered, this, &ABaseSelectorActor::OnPreviousActionTriggered);
	}
    
	if (ConfirmSelectionAction)
	{
		EnhancedInputComponent->BindAction(ConfirmSelectionAction, ETriggerEvent::Triggered, this, &ABaseSelectorActor::OnConfirmActionTriggered);
	}
}

void ABaseSelectorActor::UnbindInputs()
{
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (!PlayerController)
	{
		return;
	}
    
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
	if (!Subsystem)
	{
		return;
	}
    
	// Unbind actions if needed
	// The actions will be unbound automatically when the input component is destroyed,
	// but we provide this method for explicit unbinding if needed
}