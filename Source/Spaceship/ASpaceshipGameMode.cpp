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
    if (!ScoreManagerClass)
    {
        UE_LOG(LogTemp, Error, TEXT("ScoreManagerClass not set in GameMode Blueprint!"));
        return;
    }

    // Create score manager
    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = this;
    ScoreManager = GetWorld()->SpawnActor<AScoreManager>(
        ScoreManagerClass, 
        FVector::ZeroVector, 
        FRotator::ZeroRotator, 
        SpawnParams
    );

    if (!ScoreManager)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to spawn ScoreManager!"));
        return;
    }

    UE_LOG(LogTemp, Log, TEXT("ScoreManager initialized successfully"));
}
bool ASpaceshipGameMode::ValidateScoreManager() const
{
    if (!ScoreManager)
    {
        UE_LOG(LogTemp, Warning, TEXT("ScoreManager is null! This may cause scoring issues."));
        return false;
    }
    return true;
}
AScoreManager* ASpaceshipGameMode::GetScoreManager() const
{
    // Add validation
    if (!ValidateScoreManager())
    {
        UE_LOG(LogTemp, Error, TEXT("Attempting to access invalid ScoreManager!"));
    }
    return ScoreManager;
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
    // Vérifions d'abord si un HUD existe déjà pour éviter la double initialisation
    if (ActiveHUD)
    {
        UE_LOG(LogTemp, Warning, TEXT("HUD already initialized!"));
        return;
    }

    if (!PlayerController || !GameHUDWidgetClass)
    {
        UE_LOG(LogTemp, Error, TEXT("InitializeHUDForPlayer: Missing PlayerController or HUDClass"));
        return;
    }

    // Création du HUD
    ActiveHUD = CreateWidget<UWBP_HUD_Base>(PlayerController, GameHUDWidgetClass);
    if (!ActiveHUD)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to create HUD Widget"));
        return;
    }

    // Initialisation unique
    ActiveHUD->AddToViewport();
    
    // State initial 
    if (ScoreManager)
    {
        ActiveHUD->UpdateScore(0, 0); // Score initial
        ActiveHUD->UpdateCombo(0);    // Combo initial
    }

    // Health initial si nécessaire
    if (ASpaceshipCharacter* Character = GetSpaceshipCharacter())
    {
        ActiveHUD->UpdateHealth(1.0f); // Full health
    }

    // UpdateSongName si disponible
    if (USpaceshipSaveManager* SaveManager = USpaceshipSaveManager::GetSaveManager(this))
    {
        if (SaveManager->CurrentSave && SaveManager->CurrentSave->LastSong.Get())
        {
            ActiveHUD->UpdateSongName(SaveManager->CurrentSave->LastSong->GetName());
        }
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

