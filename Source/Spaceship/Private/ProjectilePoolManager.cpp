#include "ProjectilePoolManager.h"
#include "Engine/World.h"

// Constructor
AProjectilePoolManager::AProjectilePoolManager()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AProjectilePoolManager::InitializePool(TSubclassOf<AActor> ActorClass, int32 PoolSize)
{
	PooledActorClass = ActorClass;
	MaxPoolSize = PoolSize;

	if (!PooledActorClass) return;

	for (int32 i = 0; i < MaxPoolSize; i++)
	{
		AActor* NewActor = GetWorld()->SpawnActor<AActor>(PooledActorClass);
		if (NewActor)
		{
			NewActor->SetActorEnableCollision(false);
			NewActor->SetActorHiddenInGame(true);
			NewActor->SetActorTickEnabled(false);
			ObjectPool.Add(NewActor);
		}
	}
}

AActor* AProjectilePoolManager::GetPooledObject()
{
	for (AActor* Actor : ObjectPool)
	{
		if (!Actor->IsActorTickEnabled())
		{
			Actor->SetActorEnableCollision(true);
			Actor->SetActorHiddenInGame(false);
			Actor->SetActorTickEnabled(true);
			return Actor;
		}
	}
	return nullptr; // Pas d’objet disponible dans le pool
}

void AProjectilePoolManager::ReturnPooledObject(AActor* Actor)
{
	if (!Actor) return;

	Actor->SetActorEnableCollision(false);
	Actor->SetActorHiddenInGame(true);
	Actor->SetActorTickEnabled(false);
}
