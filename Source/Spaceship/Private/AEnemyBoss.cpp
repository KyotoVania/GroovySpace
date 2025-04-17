#include "AEnemyBoss.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "AScoreManager.h"
#include "AVisualizerActor.h"
#include "Spaceship/ASpaceshipGameMode.h"
#include "Spaceship/NiagaraProjectile.h"

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
	if (Hitbox)
	{
        Hitbox->OnComponentHit.AddDynamic(this, &AEnemyBoss::OnBossHit);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("AEnemyBoss: Hitbox is null in BeginPlay!"));
	}
	
	// Vérifie que la santé est correcte
	CurrentHealth = FMath::Clamp(CurrentHealth, 0.0f, MaxHealth);
}
ASpaceshipGameMode* AEnemyBoss::GetSpaceshipGameMode() const
{
	if (UWorld* World = GetWorld())
	{
		return Cast<ASpaceshipGameMode>(World->GetAuthGameMode());
	}
	UE_LOG(LogTemp, Warning, TEXT("%s: Failed to get GameMode!"), *GetName());
	return nullptr;
}

void AEnemyBoss::OnBossHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    // Check if the actor that hit us is our NiagaraProjectile
    ANiagaraProjectile* Projectile = Cast<ANiagaraProjectile>(OtherActor);
    if (Projectile ) // Make sure it's a projectile and not ourselves
    {
        // Apply damage to ourselves using the standard ApplyDamage function
        // The projectile should have its damage value set
        UGameplayStatics::ApplyDamage(
            this,                           // The actor receiving damage (the boss itself)
            Projectile->Damage,             // Get damage value from the projectile
            Projectile->GetInstigatorController(), // Get the controller that fired the projectile
            Projectile,                     // The actor that caused the damage (the projectile)
            UDamageType::StaticClass()      // Use a default damage type
        );

        // Note: The projectile should handle its own destruction and impact FX
        // in its OWN OnHit function. We don't destroy the projectile here.
    }
    // You could add checks here for other types of actors hitting the boss if needed
}


// Standard TakeDamage override - This is called INTERNALLY by ApplyDamage
float AEnemyBoss::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, 
							class AController* EventInstigator, AActor* DamageCauser)
{
	const float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (ActualDamage > 0.0f)
	{
		CurrentHealth = FMath::Clamp(CurrentHealth - ActualDamage, 0.0f, MaxHealth);

		if (HitSound && CurrentHealth > 0.0f)
		{
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), HitSound, GetActorLocation());
		}

		// Use GameMode to access ScoreManager
		if (ASpaceshipGameMode* GameMode = GetSpaceshipGameMode())
		{
			if (AScoreManager* ScoreManager = GameMode->GetScoreManager())
			{
				ScoreManager->AddScore(10);
				ScoreManager->IncrementCombo();
			}
		}

		if (IsDestroyed())
		{
			HandleDeath();
		}
	}

	return ActualDamage;
}

// NEW Function to handle death logic cleanly
void AEnemyBoss::HandleDeath()
{
    UE_LOG(LogTemp, Log, TEXT("AEnemyBoss: Boss destroyed!"));

    // Play death sound
    if (DeathSound)
    {
        UGameplayStatics::PlaySoundAtLocation(GetWorld(), DeathSound, GetActorLocation());
    }
	
    // Add final score bonus
	ASpaceshipGameMode* GameMode = GetSpaceshipGameMode();
	AScoreManager* ScoreManager = GameMode ? GameMode->GetScoreManager() : nullptr;
    if(ScoreManager)
    {
        ScoreManager->AddScore(100); // TODO: Use appropriate score value
    }

    // Handle respawn via VisualizerActor
    AActor* Parent = GetAttachParentActor();
    if (Parent)
    {
        AVisualizerActor* Visualizer = Cast<AVisualizerActor>(Parent);
        if (Visualizer)
        {
            Visualizer->SpawnBoss(); // Tell the visualizer to spawn a new one
        }
    }

    // Destroy this boss instance
    Destroy();
}


bool AEnemyBoss::IsDestroyed() const
{
    return CurrentHealth <= 0.0f;
}
