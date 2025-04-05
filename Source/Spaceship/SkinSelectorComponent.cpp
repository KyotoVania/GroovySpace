#include "SkinSelectorComponent.h"


USkinSelectorComponent::USkinSelectorComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void USkinSelectorComponent::BeginPlay()
{
    Super::BeginPlay();
    
    // Ensure we have at least one skin
    if (AvailableSkins.Num() == 0)
    {
        FSkinMaterialPair DefaultSkin;
        AvailableSkins.Add(DefaultSkin);
        UE_LOG(LogTemp, Warning, TEXT("No skins defined in SkinSelectorComponent, adding default empty skin"));
    }
    
    // Validate saved skin ID
    if (SaveManager && SaveManager->CurrentSave)
    {
        // Ensure saved index is within bounds
        if (SaveManager->CurrentSave->ColorSkinID >= AvailableSkins.Num() || SaveManager->CurrentSave->ColorSkinID < 0)
        {
            SaveManager->CurrentSave->ColorSkinID = 0;
            SaveManager->SaveGame();
        }
    }
}

FSkinMaterialPair USkinSelectorComponent::GetCurrentSkin() const
{
    if (!SaveManager || !SaveManager->CurrentSave || AvailableSkins.Num() == 0)
    {
        return FSkinMaterialPair();
    }
    
    int32 SkinIndex = SaveManager->CurrentSave->ColorSkinID;
    SkinIndex = FMath::Clamp(SkinIndex, 0, AvailableSkins.Num() - 1);
    
    return AvailableSkins[SkinIndex];
}

FMaterialSet USkinSelectorComponent::GetBossVisualMaterials() const
{
    return GetCurrentSkin().BossVisualMaterials;
}

FMaterialSet USkinSelectorComponent::GetPlayerMaterials() const
{
    return GetCurrentSkin().PlayerMaterials;
}

void USkinSelectorComponent::NextSkin()
{
    if (!SaveManager || !SaveManager->CurrentSave || AvailableSkins.Num() == 0)
    {
        return;
    }
    
    SaveManager->CurrentSave->ColorSkinID = IncrementIndex(SaveManager->CurrentSave->ColorSkinID, AvailableSkins.Num());
    UpdateSaveData();
    PlaySelectionChangedSound();
}

void USkinSelectorComponent::PreviousSkin()
{
    if (!SaveManager || !SaveManager->CurrentSave || AvailableSkins.Num() == 0)
    {
        return;
    }
    
    SaveManager->CurrentSave->ColorSkinID = DecrementIndex(SaveManager->CurrentSave->ColorSkinID, AvailableSkins.Num());
    UpdateSaveData();
    PlaySelectionChangedSound();
}

void USkinSelectorComponent::UpdateSaveData()
{
    if (SaveManager)
    {
        SaveManager->SaveGame();
    }
}