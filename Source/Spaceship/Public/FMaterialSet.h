#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "FMaterialSet.generated.h"

USTRUCT(BlueprintType)
struct FMaterialSet
{
	GENERATED_BODY()

public:
	// Matériau blanc
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMaterialInterface* WhiteMaterial;

	// Matériau noir
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMaterialInterface* BlackMaterial;
};
