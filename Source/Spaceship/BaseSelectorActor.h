#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpaceshipSaveManager.h"
#include "Sound/SoundBase.h"
#include "InputActionValue.h"
#include "BaseSelectorActor.generated.h"

UCLASS(Abstract)
class SPACESHIP_API ABaseSelectorActor : public AActor
{
	GENERATED_BODY()
    
public:    
	ABaseSelectorActor();

	// Sound effects
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	USoundBase* SelectionChangedSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	USoundBase* SelectionConfirmedSound;

	// Root component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* RootSceneComponent;

	// Text component for title
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UTextRenderComponent* TitleText;
    
	// Input action references
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	class UInputAction* SelectNextAction;
    
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	class UInputAction* SelectPreviousAction;
    
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	class UInputAction* ConfirmSelectionAction;

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
	USpaceshipSaveManager* SaveManager;
    
	// Update UI elements
	UFUNCTION(BlueprintCallable, Category = "UI")
	virtual void UpdateUI() {}
    
	// Input action handlers
	UFUNCTION(BlueprintCallable, Category = "Input")
	virtual void OnNextActionTriggered(const FInputActionValue& Value) {}
    
	UFUNCTION(BlueprintCallable, Category = "Input")
	virtual void OnPreviousActionTriggered(const FInputActionValue& Value) {}
    
	UFUNCTION(BlueprintCallable, Category = "Input")
	virtual void OnConfirmActionTriggered(const FInputActionValue& Value) {}
    
	// Setup input bindings
	UFUNCTION(BlueprintCallable, Category = "Input")
	virtual void SetupInputBindings(class APlayerController* PlayerController, class UInputComponent* PlayerInputComponent);
    
	// Unbind inputs
	UFUNCTION(BlueprintCallable, Category = "Input")
	virtual void UnbindInputs();
};