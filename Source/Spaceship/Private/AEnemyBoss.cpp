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

void AEnemyBoss::TakeDamage(float Damage)
{
	// Réduire la vie
	CurrentHealth -= Damage;
	CurrentHealth = FMath::Clamp(CurrentHealth, 0.0f, MaxHealth);

	// Incrémenter le score et le combo
	
	if (ScoreManager)
	{
		ScoreManager->AddScore(10); // Exemple : 10 points par tir
		ScoreManager->IncrementCombo();
		// Jouer le son de hit
		if (HitSound && !IsDestroyed())
		{
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), HitSound, GetActorLocation());
		}
	}

	// Vérifie si le boss est détruit
	if (IsDestroyed())
	{
		//check if the actor is the child of a AVisualizerActor and call the function to spawn the boss
		AActor* Parent = GetAttachParentActor();
		if (Parent)
		{
			AVisualizerActor* Visualizer = Cast<AVisualizerActor>(Parent);
			if (Visualizer)
			{
				Destroy();
				Visualizer->SpawnBoss();
				ScoreManager->AddScore(2500); // Exemple : 10 points par tir
				// Jouer le son de destruction
				if (DeathSound)
				{
					UGameplayStatics::PlaySoundAtLocation(GetWorld(), DeathSound, GetActorLocation());
				}
			}
		}
		UE_LOG(LogTemp, Log, TEXT("Boss destroyed!"));
	}
}


bool AEnemyBoss::IsDestroyed() const
{
	return CurrentHealth <= 0.0f;
}
