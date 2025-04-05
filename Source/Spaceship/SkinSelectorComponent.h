#pragma once

#include "CoreMinimal.h"
#include "BaseSelectorComponent.h"
#include "FMaterialSet.h"
#include "SkinSelectorComponent.generated.h"

/**
 * Struct to store a pair of material sets - one for boss/visualizer and one for player
 */
USTRUCT(BlueprintType)
struct FSkinMaterialPair
{
	GENERATED_BODY()

	// Materials for boss/visualizer
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Materials")
	FMaterialSet BossVisualMaterials;

	// Materials for player spaceship
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Materials")
	FMaterialSet PlayerMaterials;
};

/**
 * Component for selecting and managing player and boss skin materials
 */
UCLASS(ClassGroup=(UI), meta=(BlueprintSpawnableComponent))
class SPACESHIP_API USkinSelectorComponent : public UBaseSelectorComponent
{
	GENERATED_BODY()

public:
	USkinSelectorComponent();

	// Available skin pairs
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Materials")
	TArray<FSkinMaterialPair> AvailableSkins;

	// Get currently selected skin
	UFUNCTION(BlueprintCallable, Category = "Materials")
	FSkinMaterialPair GetCurrentSkin() const;

	// Get boss/visualizer materials
	UFUNCTION(BlueprintCallable, Category = "Materials")
	FMaterialSet GetBossVisualMaterials() const;

	// Get player materials
	UFUNCTION(BlueprintCallable, Category = "Materials")
	FMaterialSet GetPlayerMaterials() const;

	// Increment skin selection
	UFUNCTION(BlueprintCallable, Category = "Selection")
	void NextSkin();

	// Decrement skin selection
	UFUNCTION(BlueprintCallable, Category = "Selection")
	void PreviousSkin();

protected:
	virtual void BeginPlay() override;

private:
	// Update the save file with current selection
	void UpdateSaveData();
};