#include "ASpaceshipGameMode.h"
#include "SpaceshipCharacter.h"
#include "SpaceshipSaveManager.h"
#include "Kismet/GameplayStatics.h"

ASpaceshipGameMode::ASpaceshipGameMode()
{
    // Les classes seront assignées dans le Blueprint
}

void ASpaceshipGameMode::BeginPlay()
{
    Super::BeginPlay();

    // Vérifier que les classes nécessaires sont bien assignées
    if (!SpaceshipCharacterClass)
    {
        UE_LOG(LogTemp, Error, TEXT("SpaceshipCharacterClass n'est pas assignée dans le Blueprint du GameMode!"));
    }

    if (!GameHUDWidgetClass)
    {
        UE_LOG(LogTemp, Error, TEXT("GameHUDWidgetClass n'est pas assignée dans le Blueprint du GameMode!"));
    }

    if (!ScoreManagerClass)
    {
        UE_LOG(LogTemp, Error, TEXT("ScoreManagerClass n'est pas assignée dans le Blueprint du GameMode!"));
    }

    // Définir la classe de pawn par défaut
    DefaultPawnClass = SpaceshipCharacterClass;

    // Initialize score manager
    InitializeScoreManager();
}

void ASpaceshipGameMode::InitializeScoreManager()
{
    if (!ScoreManagerClass) return;

    // Spawn score manager
    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = this;
    ScoreManager = GetWorld()->SpawnActor<AScoreManager>(ScoreManagerClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);

    if (!ScoreManager)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to spawn ScoreManager!"));
    }
}

void ASpaceshipGameMode::PostLogin(APlayerController* NewPlayer)
{
    Super::PostLogin(NewPlayer);
    InitializeHUDForPlayer(NewPlayer);
}

void ASpaceshipGameMode::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
    Super::HandleStartingNewPlayer_Implementation(NewPlayer);

    // Ensure HUD is initialized
    InitializeHUDForPlayer(NewPlayer);

    // Load and apply saved data
    USpaceshipSaveManager* SaveManager = USpaceshipSaveManager::GetSaveManager(this);
    if (SaveManager && SaveManager->CurrentSave)
    {
        // Update HUD with saved data
        if (ActiveHUD)
        {
            if (SaveManager->CurrentSave->LastSong.Get())
            {
                ActiveHUD->UpdateSongName(SaveManager->CurrentSave->LastSong.Get()->GetName());
                // Also update high score
                int32 HighScore = SaveManager->GetBestScore(SaveManager->CurrentSave->LastSong.Get());
                ActiveHUD->UpdateScore(0, HighScore); // Initial score is 0
            }
        }
    }
}

void ASpaceshipGameMode::InitializeHUDForPlayer(APlayerController* PlayerController)
{
    if (!PlayerController || !GameHUDWidgetClass) 
    {
        UE_LOG(LogTemp, Warning, TEXT("Impossible d'initialiser le HUD: PlayerController ou GameHUDWidgetClass manquant"));
        return;
    }

    // Create HUD widget
    ActiveHUD = CreateWidget<UWBP_HUD_Base>(PlayerController, GameHUDWidgetClass);
    if (ActiveHUD)
    {
        ActiveHUD->AddToViewport();

        // Initialize with default values
        if (ASpaceshipCharacter* SpaceshipChar = GetSpaceshipCharacter())
        {
            ActiveHUD->UpdateHealth(1.0f); // Full health initially
        }

        // Initialize score display
        if (ScoreManager)
        {
            ActiveHUD->UpdateScore(0, 0); // Initial scores
            ActiveHUD->UpdateCombo(0);    // Initial combo
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Échec de la création du widget HUD"));
    }
}

UWBP_HUD_Base* ASpaceshipGameMode::GetGameHUD() const
{
    return ValidateHUDOperation() ? ActiveHUD : nullptr;
}

ASpaceshipCharacter* ASpaceshipGameMode::GetSpaceshipCharacter() const
{
    APlayerController* PC = Cast<APlayerController>(GEngine->GetFirstLocalPlayerController(GetWorld()));
    if (!PC) return nullptr;

    return Cast<ASpaceshipCharacter>(PC->GetPawn());
}

bool ASpaceshipGameMode::ValidateHUDOperation() const
{
    if (!ActiveHUD)
    {
        UE_LOG(LogTemp, Warning, TEXT("HUD is not initialized!"));
        return false;
    }

    if (!GetSpaceshipCharacter())
    {
        UE_LOG(LogTemp, Warning, TEXT("SpaceshipCharacter is not present!"));
        return false;
    }

    return true;
}