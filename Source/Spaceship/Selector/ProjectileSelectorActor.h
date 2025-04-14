#pragma once

#include "CoreMinimal.h"
#include "BaseSelectorActor.h"
#include "Spaceship/FVFXOption.h" // Inclure notre nouvelle structure et DataAsset
#include "NiagaraComponent.h"
#include "Components/TextRenderComponent.h"
#include "ProjectileSelectorActor.generated.h"

UCLASS()
class SPACESHIP_API AVFXSelectorActor : public ABaseSelectorActor
{
    GENERATED_BODY()

public:
    AVFXSelectorActor();

    // Data Asset contenant toutes les options VFX disponibles
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VFX")
    UVFXOptionsDataAsset* VFXOptions;

    // Composants pour la prévisualisation (optionnel mais recommandé)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components | Preview")
    UNiagaraComponent* PreviewProjectileComp;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components | Preview")
    UNiagaraComponent* PreviewImpactComp; // Pourrait être activé sur un timer ou un event

    // Texte affichant la sélection actuelle
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UTextRenderComponent* SelectionText;

    // --- Fonctions ---

    // Récupère l'option VFX actuellement sélectionnée
    UFUNCTION(BlueprintCallable, Category = "VFX")
    FVFXOption GetCurrentVFXOption() const;

    // Récupère la paire d'effets Niagara actuellement sélectionnée
    UFUNCTION(BlueprintCallable, Category = "VFX")
    FNiagaraEffectPair GetCurrentVFXPair() const;

    // Récupère l'ID (index) du VFX actuellement sélectionné
    UFUNCTION(BlueprintCallable, Category = "VFX")
    int32 GetCurrentVFXID() const;

    // Sélectionne le prochain VFX
    UFUNCTION(BlueprintCallable, Category = "Selection")
    void NextVFX();

    // Sélectionne le VFX précédent
    UFUNCTION(BlueprintCallable, Category = "Selection")
    void PreviousVFX();

    // Confirme la sélection (sauvegarde)
    UFUNCTION(BlueprintCallable, Category = "Selection")
    void ConfirmSelection();

protected:
    virtual void BeginPlay() override;

    // Met à jour l'UI (texte, prévisualisation)
    virtual void UpdateUI() override;

    // Gère les inputs (hérités de BaseSelectorActor)
    virtual void OnNextActionTriggered(const FInputActionValue& Value) override;
    virtual void OnPreviousActionTriggered(const FInputActionValue& Value) override;
    virtual void OnConfirmActionTriggered(const FInputActionValue& Value) override;

private:
    // Met à jour le fichier de sauvegarde
    void UpdateSaveData();
};