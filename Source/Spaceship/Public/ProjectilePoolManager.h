#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProjectilePoolManager.generated.h"

UCLASS()
class SPACESHIP_API AProjectilePoolManager : public AActor
{
	GENERATED_BODY()

public:
	// Constructor
	AProjectilePoolManager();

	// Fonction pour initialiser le pool
	UFUNCTION(BlueprintCallable, Category = "PoolManager")
	void InitializePool(TSubclassOf<AActor> ActorClass, int32 PoolSize);

	// Fonction pour obtenir un objet du pool
	UFUNCTION(BlueprintCallable, Category = "PoolManager")
	AActor* GetPooledObject();

	// Fonction pour retourner un objet au pool
	UFUNCTION(BlueprintCallable, Category = "PoolManager")
	void ReturnPooledObject(AActor* Actor);

	
	// Durée avant désactivation automatique (en secondes)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pool")
	float DeactivationDelay = 5.0f; // Par défaut, 5 secondes.
	
protected:
	// Classe d’acteurs à gérer
	UPROPERTY()
	TSubclassOf<AActor> PooledActorClass;

	// Pool d’acteurs
	UPROPERTY()
	TArray<AActor*> ObjectPool;

	// Taille du pool
	int32 MaxPoolSize;
};
