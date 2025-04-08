#include "AObjectPoolManager.h"

#include "ASoundSphere.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"

AObjectPoolManager::AObjectPoolManager()
{
	PrimaryActorTick.bCanEverTick = false; // Pas besoin de tick
}

//begin to play InitializePool

void AObjectPoolManager::BeginPlay()
{
	Super::BeginPlay();
	InitializePool(PooledActorClass, PoolSize);
}

void AObjectPoolManager::InitializePool(TSubclassOf<AActor> ActorClass, int32 Size)
{
	if (!ActorClass || Size <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid ActorClass or PoolSize"));
		return;
	}

	PooledActorClass = ActorClass;
	PoolSize = Size;

	// Remplir le pool avec des objets inactifs
	for (int32 i = 0; i < PoolSize; ++i)
	{
		AActor* NewActor = GetWorld()->SpawnActor<AActor>(PooledActorClass, FVector::ZeroVector, FRotator::ZeroRotator);
		if (NewActor)
		{
			NewActor->SetActorHiddenInGame(true);
			NewActor->SetActorEnableCollision(false);
			NewActor->Tags.Add(FName("Inactive")); // Ajouter un tag "Inactive"
			ObjectPool.Add(NewActor);
		}
	}
}

AActor* AObjectPoolManager::GetPooledObject()
{
	for (AActor* Actor : ObjectPool)
	{
		ASoundSphere* SoundSphere = Cast<ASoundSphere>(Actor);
		if (Actor && Actor->Tags.Contains(FName("Inactive")) && (!SoundSphere || !SoundSphere->bIsActive))
		{
			// Reset actor state
			Actor->SetActorHiddenInGame(false);
			Actor->SetActorEnableCollision(true);
            
			// Reset component state via interface
			if (SoundSphere)
			{
				SoundSphere->Reset();
			}

			// Update tags
			Actor->Tags.Remove(FName("Inactive"));
			Actor->Tags.Add(FName("Active"));
            
			return Actor;
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("No available objects in the pool. Consider increasing the PoolSize."));
	//create a new actor if the pool is empty
	if (PooledActorClass)
	{
		AActor* NewActor = GetWorld()->SpawnActor<AActor>(PooledActorClass, FVector::ZeroVector, FRotator::ZeroRotator);
		if (NewActor)
		{
			NewActor->SetActorHiddenInGame(false);
			NewActor->SetActorEnableCollision(true);
			NewActor->Tags.Add(FName("Active")); // Ajouter un tag "Active"
			ObjectPool.Add(NewActor);
			return NewActor;
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("No available objects in the pool. Consider increasing the PoolSize."));
	return nullptr;
}
void AObjectPoolManager::ReturnPooledObject(AActor* Actor)
{
	if (!Actor || !ObjectPool.Contains(Actor))
	{
		return;
	}

	// Call StopMovement if it's a SoundSphere
	ASoundSphere* SoundSphere = Cast<ASoundSphere>(Actor);
	if (SoundSphere)
	{
		SoundSphere->StopMovement();
	}

	Actor->SetActorHiddenInGame(true);
	Actor->SetActorEnableCollision(false);
	Actor->Tags.Remove(FName("Active"));
	Actor->Tags.Add(FName("Inactive"));

}

void AObjectPoolManager::AutoDeactivate(AActor* Actor)
{
	if (Actor && ObjectPool.Contains(Actor) && Actor->Tags.Contains(FName("Active")))
	{
		ReturnPooledObject(Actor);
	}
}
