#include "SkinSelectorActor.h"
#include "Components/TextRenderComponent.h"
#include "Kismet/GameplayStatics.h"

ASkinSelectorActor::ASkinSelectorActor() : InputCooldown(0.3f), LastInputTime(0.0f)
{
    // Create player ship preview meshes (white variant)
    PlayerShipWhitePreview = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlayerShipWhitePreview"));
    PlayerShipWhitePreview->SetupAttachment(RootComponent);
    PlayerShipWhitePreview->SetRelativeLocation(FVector(50.0f, -50.0f, 0.0f));
    PlayerShipWhitePreview->SetRelativeRotation(FRotator(0.0f, 45.0f, 0.0f));
    PlayerShipWhitePreview->SetRelativeScale3D(FVector(0.5f));
    PlayerShipWhitePreview->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    
    // Create player ship preview meshes (black variant)
    PlayerShipBlackPreview = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlayerShipBlackPreview"));
    PlayerShipBlackPreview->SetupAttachment(RootComponent);
    PlayerShipBlackPreview->SetRelativeLocation(FVector(50.0f, 50.0f, 0.0f));
    PlayerShipBlackPreview->SetRelativeRotation(FRotator(0.0f, 45.0f, 0.0f));
    PlayerShipBlackPreview->SetRelativeScale3D(FVector(0.5f));
    PlayerShipBlackPreview->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    
    // Create projectile preview meshes (white variant)
    ProjectileWhitePreview = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileWhitePreview"));
    ProjectileWhitePreview->SetupAttachment(RootComponent);
    ProjectileWhitePreview->SetRelativeLocation(FVector(-50.0f, -50.0f, 0.0f));
    ProjectileWhitePreview->SetRelativeRotation(FRotator(0.0f, 45.0f, 0.0f));
    ProjectileWhitePreview->SetRelativeScale3D(FVector(0.5f));
    ProjectileWhitePreview->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    
    // Create projectile preview meshes (black variant)
    ProjectileBlackPreview = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileBlackPreview"));
    ProjectileBlackPreview->SetupAttachment(RootComponent);
    ProjectileBlackPreview->SetRelativeLocation(FVector(-50.0f, 50.0f, 0.0f));
    ProjectileBlackPreview->SetRelativeRotation(FRotator(0.0f, 45.0f, 0.0f));
    ProjectileBlackPreview->SetRelativeScale3D(FVector(0.5f));
    ProjectileBlackPreview->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    
    // Create selection text
    SelectionText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("SelectionText"));
    SelectionText->SetupAttachment(RootComponent);
    SelectionText->SetRelativeLocation(FVector(0.0f, 0.0f, 50.0f));
    SelectionText->SetHorizontalAlignment(EHTA_Center);
    SelectionText->SetWorldSize(15.0f);
    SelectionText->SetTextRenderColor(FColor::Yellow);
    
    // Set title text
    TitleText->SetText(FText::FromString(TEXT("Skin Selector")));
    TitleText->SetRelativeLocation(FVector(0.0f, 0.0f, 100.0f));
}
void ASkinSelectorActor::BeginPlay()
{
    Super::BeginPlay();
    


    // --- DEBUT VERIFICATION ---
    if (!SkinOptions)
    {
        // !!! Si ce message apparait, l'asset N'EST PAS assigné correctement !!!
        UE_LOG(LogTemp, Error, TEXT("SkinSelectorActor '%s': !!! SkinOptions est NULL dans BeginPlay! Vérifiez l'assignation dans le Blueprint ou l'instance du niveau."), *GetName());
        // On pourrait arrêter ici pour éviter d'aller plus loin
        check(SkinOptions != nullptr); // Force un crash plus explicite si c'est null
        return;
    }

    // Vérifie si l'asset assigné est vide
    if (SkinOptions->AvailableSkins.Num() == 0)
    {
        // !!! Si ce message apparait, l'asset est assigné mais VIDE !!!
        UE_LOG(LogTemp, Warning, TEXT("SkinSelectorActor '%s': SkinOptions DataAsset '%s' est assigné mais n'a AUCUN skin défini !"), *GetName(), *SkinOptions->GetName());
        // Est-ce que InitializeDefaultMaterials doit vraiment tourner ici ?
        InitializeDefaultMaterials(); // <- Appel potentiellement problématique même sans NewObject
    }
    else
    {
        // Cas normal et attendu
        UE_LOG(LogTemp, Log, TEXT("SkinSelectorActor '%s': SkinOptions DataAsset '%s' chargé avec %d skins."), *GetName(), *SkinOptions->GetName(), SkinOptions->AvailableSkins.Num());
    }
    // Set meshes from data asset
    if (SkinOptions)
    {
        if (SkinOptions->PlayerShipMesh)
        {
            PlayerShipWhitePreview->SetStaticMesh(SkinOptions->PlayerShipMesh);
            PlayerShipBlackPreview->SetStaticMesh(SkinOptions->PlayerShipMesh);
        }
        
        if (SkinOptions->ProjectileMesh)
        {
            ProjectileWhitePreview->SetStaticMesh(SkinOptions->ProjectileMesh);
            ProjectileBlackPreview->SetStaticMesh(SkinOptions->ProjectileMesh);
        }
    }
    
    // Validate saved skin ID
    if (SaveManager && SaveManager->CurrentSave)
    {
        int32 SkinCount = SkinOptions ? SkinOptions->AvailableSkins.Num() : 0;
        
        // Ensure saved index is within bounds
        if (SaveManager->CurrentSave->ColorSkinID >= SkinCount || SaveManager->CurrentSave->ColorSkinID < 0)
        {
            SaveManager->CurrentSave->ColorSkinID = 0;
            SaveManager->SaveGame();
        }
    }
    
    // Set up input bindings
    APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (PlayerController)
    {
        SetupInputBindings(PlayerController, PlayerController->InputComponent);
        UE_LOG(LogTemp, Warning, TEXT("Input bindings set up for SkinSelectorActor"));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to get PlayerController in SkinSelectorActor"));
    }
    
    // Update UI with initial values
    UpdateUI();
}

void ASkinSelectorActor::InitializeDefaultMaterials()
{
}

FSkinOption ASkinSelectorActor::GetCurrentSkin() const
{
    if (!SkinOptions || SkinOptions->AvailableSkins.Num() == 0 || !SaveManager || !SaveManager->CurrentSave)
    {
        return FSkinOption();
    }
    
    int32 SkinIndex = SaveManager->CurrentSave->ColorSkinID;
    SkinIndex = FMath::Clamp(SkinIndex, 0, SkinOptions->AvailableSkins.Num() - 1);
    
    return SkinOptions->AvailableSkins[SkinIndex];
}

FMaterialSet ASkinSelectorActor::GetGameplayMaterialSets() const // <- Change TArray<FMaterialSet> en FMaterialSet
{
    return GetCurrentSkin().GameplayMaterialSets; // Retourne directement la struct
}

FMaterialSet ASkinSelectorActor::GetProjectileMaterialSets() const // <- Change TArray<FMaterialSet> en FMaterialSet
{
    return GetCurrentSkin().ProjectileMaterialSets; // Retourne directement la struct
}

void ASkinSelectorActor::NextSkin()
{
    if (!SkinOptions || SkinOptions->AvailableSkins.Num() == 0 || !SaveManager || !SaveManager->CurrentSave)
    {
        return;
    }
    
    SaveManager->CurrentSave->ColorSkinID = IncrementIndex(SaveManager->CurrentSave->ColorSkinID, SkinOptions->AvailableSkins.Num());
    UpdateSaveData();
    UpdateUI();
    PlaySelectionChangedSound();
}

void ASkinSelectorActor::PreviousSkin()
{
    if (!SkinOptions || SkinOptions->AvailableSkins.Num() == 0 || !SaveManager || !SaveManager->CurrentSave)
    {
        return;
    }
    
    SaveManager->CurrentSave->ColorSkinID = DecrementIndex(SaveManager->CurrentSave->ColorSkinID, SkinOptions->AvailableSkins.Num());
    UpdateSaveData();
    UpdateUI();
    PlaySelectionChangedSound();
}

void ASkinSelectorActor::ConfirmSelection()
{
    if (SaveManager)
    {
        SaveManager->SaveGame();
    }
    
    PlaySelectionConfirmedSound();
}

void ASkinSelectorActor::UpdateUI()
{
    FSkinOption CurrentSkin = GetCurrentSkin();
    
    // Update selection text
    if (SelectionText)
    {
        int32 CurrentIndex = SaveManager ? SaveManager->CurrentSave->ColorSkinID : 0;
        int32 TotalSkins = SkinOptions ? SkinOptions->AvailableSkins.Num() : 0;
        
        FString SkinName = CurrentSkin.DisplayName.IsEmpty() ? 
            FString::Printf(TEXT("Skin %d"), CurrentIndex + 1) : CurrentSkin.DisplayName;
            
        SelectionText->SetText(FText::Format(
            FText::FromString(TEXT("{0} ({1}/{2})")), 
            FText::FromString(SkinName),
            FText::AsNumber(CurrentIndex + 1), 
            FText::AsNumber(TotalSkins)
        ));
    }
    
    // !! ACCES DIRECT aux membres de la struct FMaterialSet !!
    if (PlayerShipWhitePreview)
    {
        if (CurrentSkin.GameplayMaterialSets.WhiteMaterial) // Accès direct
        {
            PlayerShipWhitePreview->SetMaterial(0, CurrentSkin.GameplayMaterialSets.WhiteMaterial);
        }
    }

    if (PlayerShipBlackPreview)
    {
        if (CurrentSkin.GameplayMaterialSets.BlackMaterial) // Accès direct
        {
            PlayerShipBlackPreview->SetMaterial(0, CurrentSkin.GameplayMaterialSets.BlackMaterial);
        }
    }

    // Update projectile preview materials
    // !! ACCES DIRECT aux membres de la struct FMaterialSet !!
    if (ProjectileWhitePreview)
    {
        if (CurrentSkin.ProjectileMaterialSets.WhiteMaterial) // Accès direct
        {
            ProjectileWhitePreview->SetMaterial(0, CurrentSkin.ProjectileMaterialSets.WhiteMaterial);
        }
    }

    if (ProjectileBlackPreview)
    {
        if (CurrentSkin.ProjectileMaterialSets.BlackMaterial) // Accès direct
        {
            ProjectileBlackPreview->SetMaterial(0, CurrentSkin.ProjectileMaterialSets.BlackMaterial);
        }
    }
}

void ASkinSelectorActor::UpdateSaveData()
{
    if (SaveManager)
    {
        SaveManager->SaveGame();
    }
}
void ASkinSelectorActor::OnNextActionTriggered(const FInputActionValue& Value)
{
    float CurrentTime = GetWorld()->GetTimeSeconds();
    if (CurrentTime - LastInputTime < InputCooldown)
    {
        return; // Ignore input if cooldown is active
    }
    
    LastInputTime = CurrentTime;
    UE_LOG(LogTemp, Warning, TEXT("Next action triggered"));
    NextSkin();
}

void ASkinSelectorActor::OnPreviousActionTriggered(const FInputActionValue& Value)
{
    float CurrentTime = GetWorld()->GetTimeSeconds();
    if (CurrentTime - LastInputTime < InputCooldown)
    {
        return; // Ignore input if cooldown is active
    }
    
    LastInputTime = CurrentTime;
    UE_LOG(LogTemp, Warning, TEXT("Previous action triggered"));
    PreviousSkin();
}

void ASkinSelectorActor::OnConfirmActionTriggered(const FInputActionValue& Value)
{
    float CurrentTime = GetWorld()->GetTimeSeconds();
    if (CurrentTime - LastInputTime < InputCooldown)
    {
        return; // Ignore input if cooldown is active
    }
    
    LastInputTime = CurrentTime;
    ConfirmSelection();
}
void ASkinSelectorActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    // Only remove from root if we created it dynamically

    Super::EndPlay(EndPlayReason);
}