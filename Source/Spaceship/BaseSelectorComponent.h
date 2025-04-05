#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SpaceshipSaveManager.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"
#include "BaseSelectorComponent.generated.h"

/**
 * Base class for all selector components
 */
UCLASS(Abstract, ClassGroup=(UI), meta=(BlueprintSpawnableComponent))
class SPACESHIP_API UBaseSelectorComponent : public UActorComponent
{
	GENERATED_BODY()

public:    
	UBaseSelectorComponent();

	// Sound effects
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	TObjectPtr<USoundBase> SelectionChangedSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	TObjectPtr<USoundBase> SelectionConfirmedSound;

protected:
	virtual void BeginPlay() override;
    
	// Get the save manager
	UFUNCTION(BlueprintCallable, Category = "Save")
	USpaceshipSaveManager* GetSaveManager();
    
	// Increment an index with wrapping
	UFUNCTION(BlueprintCallable, Category = "Selection")
	int32 IncrementIndex(int32 CurrentIndex, int32 MaxIndex);
    
	// Decrement an index with wrapping
	UFUNCTION(BlueprintCallable, Category = "Selection")
	int32 DecrementIndex(int32 CurrentIndex, int32 MaxIndex);
    
	// Play selection changed sound
	UFUNCTION(BlueprintCallable, Category = "Audio")
	void PlaySelectionChangedSound();
    
	// Play selection confirmed sound
	UFUNCTION(BlueprintCallable, Category = "Audio")
	void PlaySelectionConfirmedSound();

	// Reference to the save manager
	UPROPERTY()
	TObjectPtr<USpaceshipSaveManager> SaveManager;
};