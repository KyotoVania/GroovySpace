#include "AEnemyBoss.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "AScoreManager.h"
#include "AVisualizerActor.h"

AEnemyBoss::AEnemyBoss()
{
	PrimaryActorTick.bCanEverTick = false;

	// Initialiser la hitbox
	Hitbox = CreateDefaultSubobject<UBoxComponent>(TEXT("Hitbox"));
	RootComponent = Hitbox;

	// Valeurs par défaut
	MaxHealth = 100.0f;
	CurrentHealth = MaxHealth;

}

void AEnemyBoss::BeginPlay()
{
	Super::BeginPlay();

	// Récupérer le score manager
	ScoreManager = Cast<AScoreManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AScoreManager::StaticClass()));
	//debug log de ce que contient ScoreManager
	if (ScoreManager)
	{
		UE_LOG(LogTemp, Log, TEXT("ScoreManager found!"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ScoreManager not found!"));
	}
	// Vérifie que la santé est correcte
	CurrentHealth = FMath::Clamp(CurrentHealth, 0.0f, MaxHealth);
}


bool AEnemyBoss::IsDestroyed() const
{
	return CurrentHealth <= 0.0f;
}
