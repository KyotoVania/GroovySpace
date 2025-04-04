#include "AObjectPoolManager.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"

AObjectPoolManager::AObjectPoolManager()
{
	PrimaryActorTick.bCanEverTick = false; // Pas besoin de tick
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
		// Vérifier si l'acteur a le tag "Inactive"
		if (Actor && Actor->Tags.Contains(FName("Inactive")))
		{
			Actor->SetActorHiddenInGame(false);
			Actor->SetActorEnableCollision(true);

			// Supprimer le tag "Inactive" et ajouter "Active"
			Actor->Tags.Remove(FName("Inactive"));
			Actor->Tags.Add(FName("Active"));
			// Planifier la désactivation automatique avec un TimerHandle unique
			if (DeactivationDelay > 0.0f)
			{
				FTimerHandle DeactivationTimerHandle;
				GetWorld()->GetTimerManager().SetTimer(
					DeactivationTimerHandle,
					FTimerDelegate::CreateUObject(this, &AObjectPoolManager::AutoDeactivate, Actor),
					DeactivationDelay,
					false // Pas de répétition
				);
			}
			
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
			// Planifier la désactivation automatique
			if (DeactivationDelay > 0.0f)
			{
				FTimerHandle DeactivationTimerHandle;
				GetWorld()->GetTimerManager().SetTimer(
					DeactivationTimerHandle,
					FTimerDelegate::CreateUObject(this, &AObjectPoolManager::AutoDeactivate, NewActor),
					DeactivationDelay,
					false // Pas de répétition
				);
			}
			return NewActor;
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("No available objects in the pool. Consider increasing the PoolSize."));
	return nullptr;
}

void AObjectPoolManager::ReturnPooledObject(AActor* Actor)
{
	if (Actor && ObjectPool.Contains(Actor))
	{
		Actor->SetActorHiddenInGame(true);
		Actor->SetActorEnableCollision(false);

		// Supprimer le tag "Active" et ajouter "Inactive"
		Actor->Tags.Remove(FName("Active"));
		Actor->Tags.Add(FName("Inactive"));
		UE_LOG(LogTemp, Log, TEXT("Object returned to the pool."));
		GetWorld()->GetTimerManager().ClearAllTimersForObject(Actor);
	}
}

void AObjectPoolManager::AutoDeactivate(AActor* Actor)
{
	if (Actor && ObjectPool.Contains(Actor))
	{
		ReturnPooledObject(Actor);
	}
}