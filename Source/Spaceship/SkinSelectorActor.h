#pragma once

#include "CoreMinimal.h"
#include "BaseSelectorActor.h"
#include "SkinOptionsDataAsset.h"
#include "Components/StaticMeshComponent.h"
#include "SkinSelectorActor.generated.h"

/**
 * Actor for selecting and previewing player/boss and projectile skins
 */
UCLASS()
class SPACESHIP_API ASkinSelectorActor : public ABaseSelectorActor
{
    GENERATED_BODY()

public:
    ASkinSelectorActor();

    // Options data asset containing all available skins
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skins")
    USkinOptionsDataAsset* SkinOptions;

    // Preview components for player ships (white/black variants)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UStaticMeshComponent* PlayerShipWhitePreview;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UStaticMeshComponent* PlayerShipBlackPreview;

    // Preview components for projectiles (white/black variants)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UStaticMeshComponent* ProjectileWhitePreview;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UStaticMeshComponent* ProjectileBlackPreview;

    // Selection text component
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UTextRenderComponent* SelectionText;

    // Get currently selected skin
    UFUNCTION(BlueprintCallable, Category = "Materials")
    FSkinOption GetCurrentSkin() const;

    // Get gameplay materials array (player/boss)
    UFUNCTION(BlueprintCallable, Category = "Materials")
    FMaterialSet  GetGameplayMaterialSets() const;

    // Get projectile materials array
    UFUNCTION(BlueprintCallable, Category = "Materials")
    FMaterialSet GetProjectileMaterialSets() const;

    // Increment skin selection
    UFUNCTION(BlueprintCallable, Category = "Selection")
    void NextSkin();

    // Decrement skin selection
    UFUNCTION(BlueprintCallable, Category = "Selection")
    void PreviousSkin();

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
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
    void UpdateSaveData();
    
    // Try to load the default materials if needed
    void InitializeDefaultMaterials();
    
    // Input cooldown to prevent button spamming
    float InputCooldown;
    
    // Timestamp of last input action
    float LastInputTime;
};