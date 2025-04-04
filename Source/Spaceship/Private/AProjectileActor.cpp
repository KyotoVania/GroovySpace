#include "AProjectileActor.h"
#include "ProjectilePoolManager.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"

AProjectileActor::AProjectileActor()
{
    PrimaryActorTick.bCanEverTick = true;
    // Initialisation des paramètres
    Speed = 1000.0f;
    Lifetime = 5.0f;

}



void AProjectileActor::BeginPlay()
{
    Super::BeginPlay();
}


// Activation du projectile
void AProjectileActor::ActivateProjectile(const FVector& SpawnLocation, const FRotator& SpawnRotation, float GivenSpeed)
{
    // Mise à jour de la position, rotation et vitesse
    SetActorLocation(SpawnLocation);
    SetActorRotation(SpawnRotation);
    Speed = GivenSpeed;

    // Activer le projectile
    SetActorHiddenInGame(false);
    SetActorEnableCollision(true);
    SetActorTickEnabled(true);
    this->Tags.Add(FName("Active"));
    //removing tag Inactive
    if (this->Tags.Contains(FName("Inactive")))
    {
        this->Tags.Remove(FName("Inactive"));
    }

    // Lancer le timer pour la durée de vie
}

// Désactivation du projectile
void AProjectileActor::DeactivateProjectile()
{
    // Désactiver le projectile
    SetActorHiddenInGame(true);
    SetActorEnableCollision(false);
    SetActorTickEnabled(false);

    // Retourner au Pool Manager
    if (PoolManager)
    {
        PoolManager->ReturnPooledObject(this);
    }
    else 
    {
    //add tag Inactive to the projectile otherwise 
    //it will be destroyed by the garbage collector
        this->Tags.Add(FName("Inactive"));
    }
}
