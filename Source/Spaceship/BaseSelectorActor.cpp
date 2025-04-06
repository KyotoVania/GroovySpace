#include "BaseSelectorActor.h"
#include "Components/TextRenderComponent.h"
#include "Kismet/GameplayStatics.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Components/BoxComponent.h"

ABaseSelectorActor::ABaseSelectorActor() : InputCooldown(0.3f), LastInputTime(0.0f)
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
	// <<< CREER LA TRIGGER BOX >>>
	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	TriggerBox->SetupAttachment(RootComponent); // Attache la box au root
	TriggerBox->SetCollisionProfileName(TEXT("Trigger")); // Profil de collision pour overlap seulement
	TriggerBox->SetGenerateOverlapEvents(true);
	// Optionnel : Définir une taille par défaut, ajustable dans l'éditeur
	TriggerBox->SetBoxExtent(FVector(150.0f, 150.0f, 100.0f));
}

void ABaseSelectorActor::BeginPlay()
{
	Super::BeginPlay();
	// <<< LIER LES EVENEMENTS D'OVERLAP >>>
	if (TriggerBox)
	{
		TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &ABaseSelectorActor::OnOverlapBegin);
		TriggerBox->OnComponentEndOverlap.AddDynamic(this, &ABaseSelectorActor::OnOverlapEnd);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("TriggerBox non trouvé dans %s"), *GetName());
	}
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


// --- Fonctions d'Overlap ---

void ABaseSelectorActor::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && IsPlayerPawn(OtherActor))
	{
		bPlayerIsInside = true;
		UE_LOG(LogTemp, Log, TEXT("Joueur ENTRÉ dans la zone de %s"), *GetName());
		// Optionnel : Activer visuellement quelque chose, changer l'UI, etc.
	}
}

void ABaseSelectorActor::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor && IsPlayerPawn(OtherActor))
	{
		bPlayerIsInside = false;
		UE_LOG(LogTemp, Log, TEXT("Joueur SORTI de la zone de %s"), *GetName());
		// Optionnel : Désactiver l'effet visuel, etc.
	}
}

// --- Helper pour vérifier le joueur ---
bool ABaseSelectorActor::IsPlayerPawn(AActor* ActorToCheck) const
{
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0); // Prend le joueur 0
	return (ActorToCheck != nullptr && ActorToCheck == PlayerPawn);
}