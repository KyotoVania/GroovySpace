#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AObjectPoolManager.generated.h"

/**
 * Classe générique pour gérer le pool d'objets.
 */
UCLASS()
class SPACESHIP_API AObjectPoolManager : public AActor
{
	GENERATED_BODY()

public:
	// Constructeur
	AObjectPoolManager();
	void BeginPlay();

	// Initialise le pool avec une classe d'objets et une taille
	UFUNCTION(BlueprintCallable, Category = "Object Pool")
	void InitializePool(TSubclassOf<AActor> ActorClass, int32 PoolSize);

	// Retourne un objet actif du pool
	UFUNCTION(BlueprintCallable, Category = "Object Pool")
	AActor* GetPooledObject();

	// Retourne un objet au pool
	UFUNCTION(BlueprintCallable, Category = "Object Pool")
	void ReturnPooledObject(AActor* Actor);

	// Durée avant désactivation automatique (en secondes)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pool")
	float DeactivationDelay = 5.0f; // Par défaut, 5 secondes.


protected:
	// Classe des objets à instancier dans le pool
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Object Pool")
	TSubclassOf<AActor> PooledActorClass;

	// Taille maximale du pool
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Object Pool")
	int32 PoolSize;

	// Tableau des objets du pool
	UPROPERTY(VisibleAnywhere, Category = "Object Pool")
	TArray<AActor*> ObjectPool;

private:
	// Fonction pour désactiver un objet automatiquement
	void AutoDeactivate(AActor* Actor);
};
