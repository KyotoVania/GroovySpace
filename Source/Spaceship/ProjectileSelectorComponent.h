#pragma once

#include "CoreMinimal.h"
#include "BaseSelectorComponent.h"
#include "NiagaraSystem.h"
#include "ProjectileSelectorComponent.generated.h"

/**
 * Component for selecting and managing player projectile effects
 */
UCLASS(ClassGroup=(UI), meta=(BlueprintSpawnableComponent))
class SPACESHIP_API UProjectileSelectorComponent : public UBaseSelectorComponent
{
	GENERATED_BODY()

public:
	UProjectileSelectorComponent();

	// Available projectile effects
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectiles")
	TArray<TObjectPtr<UNiagaraSystem>> ProjectileEffects;

	// Preview meshes for each projectile type (optional)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectiles")
	TArray<TObjectPtr<UStaticMesh>> ProjectilePreviewMeshes;

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

protected:
	virtual void BeginPlay() override;

private:
	// Update the save file with current selection
	void UpdateSaveData();
};