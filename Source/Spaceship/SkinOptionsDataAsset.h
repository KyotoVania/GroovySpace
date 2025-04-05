#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "FMaterialSet.h"
#include "SkinOptionsDataAsset.generated.h"

/**
 * Struct to store a complete skin option
 */
USTRUCT(BlueprintType)
struct FSkinOption
{
	GENERATED_BODY()

	// Display name for this skin
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skin")
	FString DisplayName;

	// Ship and boss materials (white/black pair)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Materials")
	FMaterialSet GameplayMaterialSets;
    
	// Projectile materials (white/black pair)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Materials")
	FMaterialSet ProjectileMaterialSets;
    
	// Is this skin locked initially?
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skin")
	bool bLockedByDefault = false;
};

/**
 * DataAsset to store all available skin options
 */
UCLASS(BlueprintType)
class SPACESHIP_API USkinOptionsDataAsset : public UDataAsset
{
	GENERATED_BODY()
    
public:
	// List of all available skin options
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skins")
	TArray<FSkinOption> AvailableSkins;
    
	// Player ship mesh to display (same for all skins)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Preview")
	UStaticMesh* PlayerShipMesh;
    
	// Projectile mesh to display (same for all skins)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Preview")
	UStaticMesh* ProjectileMesh;
    
	// Default material for white variant
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Defaults")
	UMaterialInterface* DefaultWhiteMaterial;
    
	// Default material for black variant
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Defaults")
	UMaterialInterface* DefaultBlackMaterial;
};