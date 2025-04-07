#include "ProjectileSelectorActor.h"
#include "Components/TextRenderComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"

AProjectileSelectorActor::AProjectileSelectorActor()
{
    // Create preview mesh component
    PreviewMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PreviewMesh"));
    PreviewMesh->SetupAttachment(RootComponent);
    PreviewMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    
    // Create preview effect component
    PreviewEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("PreviewEffect"));
    PreviewEffect->SetupAttachment(RootComponent);
    
    // Create direction arrow
    DirectionArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("DirectionArrow"));
    DirectionArrow->SetupAttachment(RootComponent);
    DirectionArrow->SetArrowColor(FColor::Green);
    DirectionArrow->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));
    
    // Create selection text
    SelectionText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("SelectionText"));
    SelectionText->SetupAttachment(RootComponent);
    SelectionText->SetRelativeLocation(FVector(0.0f, 0.0f, 50.0f));
    SelectionText->SetHorizontalAlignment(EHTA_Center);
    SelectionText->SetWorldSize(15.0f);
    SelectionText->SetTextRenderColor(FColor::Yellow);
    
    // Set title text
    TitleText->SetText(FText::FromString(TEXT("Projectile Selector")));
    TitleText->SetRelativeLocation(FVector(0.0f, 0.0f, 100.0f));
}

void AProjectileSelectorActor::BeginPlay()
{
    Super::BeginPlay();
    
    // Ensure we have at least one projectile effect
    if (ProjectileEffects.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("No projectile effects defined in ProjectileSelectorActor"));
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
    
    // Update UI with initial values
    UpdateUI();
}

UNiagaraSystem* AProjectileSelectorActor::GetCurrentProjectileEffect() const
{
    if (!SaveManager || !SaveManager->CurrentSave || ProjectileEffects.Num() == 0)
    {
        return nullptr;
    }
    
    int32 EffectIndex = SaveManager->CurrentSave->ProjectileSkinID;
    EffectIndex = FMath::Clamp(EffectIndex, 0, ProjectileEffects.Num() - 1);
    
    return ProjectileEffects[EffectIndex];
}

UStaticMesh* AProjectileSelectorActor::GetCurrentPreviewMesh() const
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

int32 AProjectileSelectorActor::GetCurrentProjectileID() const
{
    if (!SaveManager || !SaveManager->CurrentSave)
    {
        return 0;
    }
    
    return SaveManager->CurrentSave->ProjectileSkinID;
}

void AProjectileSelectorActor::NextProjectile()
{
    if (!SaveManager || !SaveManager->CurrentSave || ProjectileEffects.Num() == 0)
    {
        return;
    }
    
    SaveManager->CurrentSave->ProjectileSkinID = IncrementIndex(SaveManager->CurrentSave->ProjectileSkinID, ProjectileEffects.Num());
    UpdateSaveData();
    UpdateUI();
    PlaySelectionChangedSound();
}

void AProjectileSelectorActor::PreviousProjectile()
{
    if (!SaveManager || !SaveManager->CurrentSave || ProjectileEffects.Num() == 0)
    {
        return;
    }
    
    SaveManager->CurrentSave->ProjectileSkinID = DecrementIndex(SaveManager->CurrentSave->ProjectileSkinID, ProjectileEffects.Num());
    UpdateSaveData();
    UpdateUI();
    PlaySelectionChangedSound();
}

void AProjectileSelectorActor::ConfirmSelection()
{
    if (SaveManager)
    {
        SaveManager->SaveGame();
    }
    
    PlaySelectionConfirmedSound();
}

void AProjectileSelectorActor::UpdateUI()
{
    // Update selection text
    if (SelectionText)
    {
        int32 CurrentIndex = GetCurrentProjectileID();
        SelectionText->SetText(FText::Format(FText::FromString(TEXT("Projectile {0}/{1}")), 
            FText::AsNumber(CurrentIndex + 1), FText::AsNumber(ProjectileEffects.Num())));
    }
    
    // Update preview mesh
    if (PreviewMesh)
    {
        UStaticMesh* CurrentMesh = GetCurrentPreviewMesh();
        if (CurrentMesh)
        {
            PreviewMesh->SetStaticMesh(CurrentMesh);
            PreviewMesh->SetVisibility(true);
        }
        else
        {
            PreviewMesh->SetVisibility(false);
        }
    }
    
    // Update preview effect
    if (PreviewEffect)
    {
        UNiagaraSystem* CurrentEffect = GetCurrentProjectileEffect();
        if (CurrentEffect && CurrentEffect != PreviewEffect->GetAsset())
        {
            PreviewEffect->SetAsset(CurrentEffect);
            PreviewEffect->Activate(true);
        }
    }
}

void AProjectileSelectorActor::UpdateSaveData()
{
    if (SaveManager)
    {
        SaveManager->SaveGame();
    }
}

void AProjectileSelectorActor::OnNextActionTriggered(const FInputActionValue& Value)
{
    NextProjectile();
}

void AProjectileSelectorActor::OnPreviousActionTriggered(const FInputActionValue& Value)
{
    PreviousProjectile();
}

void AProjectileSelectorActor::OnConfirmActionTriggered(const FInputActionValue& Value)
{
    ConfirmSelection();
}