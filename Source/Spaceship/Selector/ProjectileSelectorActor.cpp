#include "ProjectileSelectorActor.h"
#include "Components/TextRenderComponent.h"
#include "NiagaraComponent.h"
#include "Spaceship/SpaceshipSaveManager.h" // Assurez-vous du chemin
#include "Kismet/GameplayStatics.h"
#include "InputActionValue.h" // Nécessaire pour les fonctions Triggered

AVFXSelectorActor::AVFXSelectorActor()
{
    // Créer les composants de prévisualisation Niagara
    PreviewProjectileComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("PreviewProjectileComp"));
    PreviewProjectileComp->SetupAttachment(RootComponent);
    PreviewProjectileComp->SetRelativeLocation(FVector(0.0f, -50.0f, 50.0f)); // Ajustez la position
    PreviewProjectileComp->bAutoActivate = false; // Activer manuellement

    PreviewImpactComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("PreviewImpactComp"));
    PreviewImpactComp->SetupAttachment(RootComponent);
    PreviewImpactComp->SetRelativeLocation(FVector(0.0f, 50.0f, 50.0f)); // Ajustez la position
    PreviewImpactComp->bAutoActivate = false; // Activer manuellement

    // Créer le composant texte pour la sélection
    SelectionText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("SelectionText"));
    SelectionText->SetupAttachment(RootComponent);
    SelectionText->SetRelativeLocation(FVector(0.0f, 0.0f, 70.0f)); // Position au-dessus
    SelectionText->SetHorizontalAlignment(EHTA_Center);
    SelectionText->SetWorldSize(15.0f);
    SelectionText->SetTextRenderColor(FColor::Cyan); // Couleur distinctive

    // Titre du sélecteur
    if (TitleText) // TitleText est dans BaseSelectorActor
    {
        TitleText->SetText(FText::FromString(TEXT("VFX Selector")));
        TitleText->SetRelativeLocation(FVector(0.0f, 0.0f, 100.0f)); // Position encore plus haute
    }

    // Initialisation des variables
    VFXOptions = nullptr;
}

void AVFXSelectorActor::BeginPlay()
{
    Super::BeginPlay(); // Important: Appelle le BeginPlay de BaseSelectorActor

    // Vérification du Data Asset
    if (!VFXOptions)
    {
        UE_LOG(LogTemp, Error, TEXT("VFXSelectorActor '%s': VFXOptions Data Asset n'est pas assigné !"), *GetName());
         check(VFXOptions != nullptr); // Crash pour forcer l'assignation
        return;
    }
    if (VFXOptions->AvailableVFX.Num() == 0)
    {
         UE_LOG(LogTemp, Warning, TEXT("VFXSelectorActor '%s': VFXOptions Data Asset '%s' est vide !"), *GetName(), *VFXOptions->GetName());
        // Pas besoin de continuer si pas d'options
         return;
    }
    UE_LOG(LogTemp, Log, TEXT("VFXSelectorActor '%s': Chargé avec %d options VFX."), *GetName(), VFXOptions->AvailableVFX.Num());


    // Validation de l'index sauvegardé
    if (SaveManager && SaveManager->CurrentSave)
    {
        int32& SavedID = SaveManager->CurrentSave->ProjectileSkinID; // Assurez-vous que c'est le bon ID
        int32 MaxIndex = VFXOptions->AvailableVFX.Num();

        if (SavedID < 0 || SavedID >= MaxIndex)
        {
            UE_LOG(LogTemp, Warning, TEXT("Index VFX sauvegardé (%d) invalide. Réinitialisation à 0."), SavedID);
            SavedID = 0;
            UpdateSaveData(); // Sauvegarde l'index corrigé
        }
    }
     else
    {
         UE_LOG(LogTemp, Error, TEXT("VFXSelectorActor: SaveManager ou CurrentSave est null!"));
    }

    // Mise à jour initiale de l'UI
    UpdateUI();

    // Setup input (si nécessaire, souvent fait par le Pawn qui entre dans la zone)
     APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
     if (PC && PC->InputComponent) // Vérifier aussi InputComponent
     {
         SetupInputBindings(PC, PC->InputComponent); // SetupInputBindings est dans BaseSelectorActor
         UE_LOG(LogTemp, Log, TEXT("Input Bindings setup for VFXSelectorActor via BeginPlay (vérifier si c'est la bonne méthode)"));
     }
     else
     {
         UE_LOG(LogTemp, Warning, TEXT("VFXSelectorActor: PlayerController ou son InputComponent non trouvé au BeginPlay. Input peut être géré par Overlap."));
     }
}

FVFXOption AVFXSelectorActor::GetCurrentVFXOption() const
{
    if (!VFXOptions || VFXOptions->AvailableVFX.Num() == 0 || !SaveManager || !SaveManager->CurrentSave)
    {
        // Retourne une option vide ou la première si possible pour éviter crash
        return (VFXOptions && VFXOptions->AvailableVFX.IsValidIndex(0)) ? VFXOptions->AvailableVFX[0] : FVFXOption();
    }

    int32 CurrentIndex = GetCurrentVFXID();
    if (!VFXOptions->AvailableVFX.IsValidIndex(CurrentIndex))
    {
         UE_LOG(LogTemp, Error, TEXT("GetCurrentVFXOption: Index %d invalide pour AvailableVFX (Taille: %d)"), CurrentIndex, VFXOptions->AvailableVFX.Num());
         return (VFXOptions->AvailableVFX.IsValidIndex(0)) ? VFXOptions->AvailableVFX[0] : FVFXOption();
    }

    return VFXOptions->AvailableVFX[CurrentIndex];
}

FNiagaraEffectPair AVFXSelectorActor::GetCurrentVFXPair() const
{
    // Appelle GetCurrentVFXOption et retourne sa paire d'effets
    return GetCurrentVFXOption().Effects;
}


int32 AVFXSelectorActor::GetCurrentVFXID() const
{
    if (!SaveManager || !SaveManager->CurrentSave)
    {
         UE_LOG(LogTemp, Warning, TEXT("GetCurrentVFXID: SaveManager ou CurrentSave est null. Retourne 0."));
        return 0;
    }
    // Assure que l'index est valide avant de le retourner
     int32 MaxIndex = (VFXOptions) ? VFXOptions->AvailableVFX.Num() : 0;
     if (MaxIndex == 0) return 0;
     return FMath::Clamp(SaveManager->CurrentSave->ProjectileSkinID, 0, MaxIndex - 1);
}

void AVFXSelectorActor::NextVFX()
{
    if (!VFXOptions || VFXOptions->AvailableVFX.Num() == 0 || !SaveManager || !SaveManager->CurrentSave)
    {
        return;
    }

    int32 MaxIndex = VFXOptions->AvailableVFX.Num();
    SaveManager->CurrentSave->ProjectileSkinID = IncrementIndex(SaveManager->CurrentSave->ProjectileSkinID, MaxIndex); // Utilise la fonction de base
    UpdateSaveData();
    UpdateUI();
    PlaySelectionChangedSound(); // Son de la classe de base
     UE_LOG(LogTemp, Log, TEXT("Next VFX selected. New Index: %d"), SaveManager->CurrentSave->ProjectileSkinID);
}

void AVFXSelectorActor::PreviousVFX()
{
     if (!VFXOptions || VFXOptions->AvailableVFX.Num() == 0 || !SaveManager || !SaveManager->CurrentSave)
    {
        return;
    }

    int32 MaxIndex = VFXOptions->AvailableVFX.Num();
    SaveManager->CurrentSave->ProjectileSkinID = DecrementIndex(SaveManager->CurrentSave->ProjectileSkinID, MaxIndex); // Utilise la fonction de base
    UpdateSaveData();
    UpdateUI();
    PlaySelectionChangedSound(); // Son de la classe de base
     UE_LOG(LogTemp, Log, TEXT("Previous VFX selected. New Index: %d"), SaveManager->CurrentSave->ProjectileSkinID);
}

void AVFXSelectorActor::ConfirmSelection()
{
    UpdateSaveData(); // Assure que la dernière sélection est sauvegardée
    PlaySelectionConfirmedSound(); // Son de la classe de base
     UE_LOG(LogTemp, Log, TEXT("VFX selection confirmed. Index: %d"), GetCurrentVFXID());
     // Optionnel: Ajouter un feedback visuel ou autre action de confirmation
}

void AVFXSelectorActor::UpdateUI()
{
    FVFXOption CurrentOption = GetCurrentVFXOption();
    int32 CurrentIndex = GetCurrentVFXID();
    int32 TotalOptions = (VFXOptions) ? VFXOptions->AvailableVFX.Num() : 0;

    // Met à jour le texte de sélection
    if (SelectionText)
    {
        FString DisplayName = CurrentOption.DisplayName.IsEmpty() ?
            FString::Printf(TEXT("VFX Set %d"), CurrentIndex + 1) : CurrentOption.DisplayName;

        SelectionText->SetText(FText::Format(
            FText::FromString(TEXT("{0} ({1}/{2})")),
            FText::FromString(DisplayName),
            FText::AsNumber(CurrentIndex + 1),
            FText::AsNumber(TotalOptions)
        ));
    }

    // Met à jour les previews Niagara
    if (PreviewProjectileComp)
    {
        UNiagaraSystem* ProjFX = CurrentOption.Effects.ProjectileFX; // Utilise l'effet réel pour la preview
        if (ProjFX && ProjFX != PreviewProjectileComp->GetAsset())
        {
             PreviewProjectileComp->Deactivate(); // Important avant de changer l'asset
            PreviewProjectileComp->SetAsset(ProjFX);
            PreviewProjectileComp->Activate(true); // Réactive avec le nouvel asset
             UE_LOG(LogTemp, Verbose, TEXT("Preview Projectile FX updated."));
        }
        else if (!ProjFX)
        {
             PreviewProjectileComp->Deactivate();
             PreviewProjectileComp->SetAsset(nullptr); // Vide l'asset si aucun n'est défini
             UE_LOG(LogTemp, Verbose, TEXT("Preview Projectile FX cleared."));
        }
    }

    if (PreviewImpactComp)
    {
        UNiagaraSystem* ImpactFX = CurrentOption.Effects.ImpactFX; // Utilise l'effet réel pour la preview
         // Pour l'impact, on pourrait le jouer une seule fois au lieu de le laisser actif
         if (ImpactFX && ImpactFX != PreviewImpactComp->GetAsset()) // Seulement si différent
         {
             PreviewImpactComp->SetAsset(ImpactFX);
             // Option 1: Jouer une fois
              PreviewImpactComp->Activate(true); // Joue une fois si le système est configuré pour ne pas looper
             // Option 2: Le laisser en boucle (moins idéal pour un impact)
             // PreviewImpactComp->Activate(true);
              UE_LOG(LogTemp, Verbose, TEXT("Preview Impact FX updated (played once)."));

             // Optionnel : Timer pour le désactiver après un court délai si l'effet boucle
             // FTimerHandle TempHandle;
             // GetWorldTimerManager().SetTimer(TempHandle, PreviewImpactComp, &UNiagaraComponent::Deactivate, 0.5f, false);
         }
          else if (!ImpactFX)
        {
             PreviewImpactComp->Deactivate();
             PreviewImpactComp->SetAsset(nullptr);
             UE_LOG(LogTemp, Verbose, TEXT("Preview Impact FX cleared."));
        }
    }
}

void AVFXSelectorActor::UpdateSaveData()
{
    if (SaveManager)
    {
        SaveManager->SaveGame();
         UE_LOG(LogTemp, Verbose, TEXT("VFXSelectorActor: SaveGame called."));
    }
     else
    {
         UE_LOG(LogTemp, Error, TEXT("VFXSelectorActor: Attempted to save but SaveManager is null!"));
    }
}

// --- Input Handlers ---
// Ils vérifient si le joueur est dans la zone (bPlayerIsInside de BaseSelectorActor)
// et gèrent le cooldown (LastInputTime de BaseSelectorActor)

void AVFXSelectorActor::OnNextActionTriggered(const FInputActionValue& Value)
{
    // Vérification du cooldown et de la présence du joueur
    float CurrentTime = GetWorld()->GetTimeSeconds();
    if (!bPlayerIsInside || (CurrentTime - LastInputTime < InputCooldown))
    {
         UE_LOG(LogTemp, Verbose, TEXT("Input 'Next' ignoré (Zone: %d, Cooldown: %f)"), bPlayerIsInside, CurrentTime - LastInputTime);
        return;
    }
    LastInputTime = CurrentTime;

    // Appel de la logique spécifique
    NextVFX();
}

void AVFXSelectorActor::OnPreviousActionTriggered(const FInputActionValue& Value)
{
     // Vérification du cooldown et de la présence du joueur
    float CurrentTime = GetWorld()->GetTimeSeconds();
    if (!bPlayerIsInside || (CurrentTime - LastInputTime < InputCooldown))
    {
        UE_LOG(LogTemp, Verbose, TEXT("Input 'Previous' ignoré (Zone: %d, Cooldown: %f)"), bPlayerIsInside, CurrentTime - LastInputTime);
        return;
    }
    LastInputTime = CurrentTime;

    // Appel de la logique spécifique
    PreviousVFX();
}

void AVFXSelectorActor::OnConfirmActionTriggered(const FInputActionValue& Value)
{
    // Vérification du cooldown et de la présence du joueur
    float CurrentTime = GetWorld()->GetTimeSeconds();
    if (!bPlayerIsInside || (CurrentTime - LastInputTime < InputCooldown))
    {
         UE_LOG(LogTemp, Verbose, TEXT("Input 'Confirm' ignoré (Zone: %d, Cooldown: %f)"), bPlayerIsInside, CurrentTime - LastInputTime);
        return;
    }
    LastInputTime = CurrentTime;

    // Appel de la logique spécifique
    ConfirmSelection();
}