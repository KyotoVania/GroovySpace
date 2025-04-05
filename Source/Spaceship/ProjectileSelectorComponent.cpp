#include "ProjectileSelectorComponent.h"

UProjectileSelectorComponent::UProjectileSelectorComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UProjectileSelectorComponent::BeginPlay()
{
    Super::BeginPlay();
    
    // Ensure we have at least one projectile effect
    if (ProjectileEffects.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("No projectile effects defined in ProjectileSelectorComponent"));
    }
    
    // Validate saved projectile ID
    if (SaveManager && SaveManager->CurrentSave)
    {
        // Ensure saved index is within bounds
        if (SaveManager->CurrentSave->ProjectileSkinID >= ProjectileEffects.Num() || SaveManager->CurrentSave->ProjectileSkinID < 0)
        {
            SaveManager->CurrentSave->ProjectileSkinID = 0;
            SaveManager->SaveGame();
        }
    }
}

UNiagaraSystem* UProjectileSelectorComponent::GetCurrentProjectileEffect() const
{
    if (!SaveManager || !SaveManager->CurrentSave || ProjectileEffects.Num() == 0)
    {
        return nullptr;
    }
    
    int32 EffectIndex = SaveManager->CurrentSave->ProjectileSkinID;
    EffectIndex = FMath::Clamp(EffectIndex, 0, ProjectileEffects.Num() - 1);
    
    return ProjectileEffects[EffectIndex];
}

UStaticMesh* UProjectileSelectorComponent::GetCurrentPreviewMesh() const
{
    if (!SaveManager || !SaveManager->CurrentSave || ProjectilePreviewMeshes.Num() == 0)
    {
        return nullptr;
    }
    
    int32 MeshIndex = SaveManager->CurrentSave->ProjectileSkinID;
    
    // Check if preview meshes array has enough elements
    if (ProjectilePreviewMeshes.Num() <= MeshIndex)
    {
        return nullptr;
    }
    
    return ProjectilePreviewMeshes[MeshIndex];
}

int32 UProjectileSelectorComponent::GetCurrentProjectileID() const
{
    if (!SaveManager || !SaveManager->CurrentSave)
    {
        return 0;
    }
    
    return SaveManager->CurrentSave->ProjectileSkinID;
}

void UProjectileSelectorComponent::NextProjectile()
{
    if (!SaveManager || !SaveManager->CurrentSave || ProjectileEffects.Num() == 0)
    {
        return;
    }
    
    SaveManager->CurrentSave->ProjectileSkinID = IncrementIndex(SaveManager->CurrentSave->ProjectileSkinID, ProjectileEffects.Num());
    UpdateSaveData();
    PlaySelectionChangedSound();
}

void UProjectileSelectorComponent::PreviousProjectile()
{
    if (!SaveManager || !SaveManager->CurrentSave || ProjectileEffects.Num() == 0)
    {
        return;
    }
    
    SaveManager->CurrentSave->ProjectileSkinID = DecrementIndex(SaveManager->CurrentSave->ProjectileSkinID, ProjectileEffects.Num());
    UpdateSaveData();
    PlaySelectionChangedSound();
}

void UProjectileSelectorComponent::UpdateSaveData()
{
    if (SaveManager)
    {
        SaveManager->SaveGame();
    }
}