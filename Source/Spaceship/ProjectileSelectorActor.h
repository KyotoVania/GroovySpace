#pragma once

#include "CoreMinimal.h"
#include "BaseSelectorActor.h"
#include "NiagaraSystem.h"
#include "Components/StaticMeshComponent.h"
#include "Components/ArrowComponent.h"
#include "NiagaraComponent.h"
#include "ProjectileSelectorActor.generated.h"

/**
 * Actor for selecting and previewing player projectile effects
 */
UCLASS()
class SPACESHIP_API AProjectileSelectorActor : public ABaseSelectorActor
{
	GENERATED_BODY()

public:
	AProjectileSelectorActor();

	// Available projectile effects
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectiles")
	TArray<UNiagaraSystem*> ProjectileEffects;

	// Preview meshes for each projectile type
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectiles")
	TArray<UStaticMesh*> ProjectilePreviewMeshes;

	// Preview mesh component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* PreviewMesh;

	// Preview effect component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UNiagaraComponent* PreviewEffect;

	// Arrow to show direction
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UArrowComponent* DirectionArrow;

	// Selection text component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UTextRenderComponent* SelectionText;

	// Get current projectile effect
	UFUNCTION(BlueprintCallable, Category = "Projectiles")
	UNiagaraSystem* GetCurrentProjectileEffect() const;

	// Get current preview mesh
	UFUNCTION(BlueprintCallable, Category = "Projectiles")
	UStaticMesh* GetCurrentPreviewMesh() const;

	// Get current projectile ID
	UFUNCTION(BlueprintCallable, Category = "Projectiles")
	int32 GetCurrentProjectileID() const;

	// Increment projectile selection
	UFUNCTION(BlueprintCallable, Category = "Selection")
	void NextProjectile();

	// Decrement projectile selection
	UFUNCTION(BlueprintCallable, Category = "Selection")
	void PreviousProjectile();

	// Confirm selection
	UFUNCTION(BlueprintCallable, Category = "Selection")
	void ConfirmSelection();

protected:
	virtual void BeginPlay() override;
	
	// Override UpdateUI to refresh the visual representation
	virtual void UpdateUI() override;
    
	// Override input handlers
	virtual void OnNextActionTriggered(const FInputActionValue& Value) override;
	virtual void OnPreviousActionTriggered(const FInputActionValue& Value) override;
	virtual void OnConfirmActionTriggered(const FInputActionValue& Value) override;

private:
	// Update the save file with current selection
	void UpdateSaveData();
};