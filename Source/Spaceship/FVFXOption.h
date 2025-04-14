// Dans un fichier .h approprié, par exemple VFXTypes.h ou dans NiagaraProjectile.h si la structure FNiagaraEffectPair y est déjà.
#pragma once

#include "CoreMinimal.h"
#include "NiagaraSystem.h"
#include "Engine/DataAsset.h" // Nécessaire pour UDataAsset plus tard
#include "NiagaraProjectile.h" // Assurez-vous que FNiagaraEffectPair est défini ici ou ailleurs
#include "FVFXOption.generated.h" // Adaptez le nom si nécessaire

// Structure pour une option VFX complète
USTRUCT(BlueprintType)
struct FVFXOption
{
	GENERATED_BODY()

	// Nom affiché dans le sélecteur
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VFX")
	FString DisplayName;

	// La paire d'effets Niagara (Projectile + Impact)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VFX")
	FNiagaraEffectPair Effects;

	// Optionnel : Prévisualisation pour le sélecteur (peut être ajouté plus tard)
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VFX | Preview")
	// UNiagaraSystem* PreviewProjectileFX;

	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VFX | Preview")
	// UNiagaraSystem* PreviewImpactFX;

	// Est-ce que ce VFX est bloqué par défaut ? (Pour des unlocks futurs)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VFX")
	bool bLockedByDefault = false;
};

// Data Asset pour stocker toutes les options VFX
UCLASS(BlueprintType)
class SPACESHIP_API UVFXOptionsDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	// Liste de toutes les options VFX disponibles
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "VFX Options")
	TArray<FVFXOption> AvailableVFX;
};