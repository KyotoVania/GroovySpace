#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UVisualizerManager.h"
#include "AEnemyBoss.h"
#include "FMaterialSet.h"
#include "ASoundSphere.h"
#include "AObjectPoolManager.h"
#include "AudioSynesthesia/Classes/ConstantQNRT.h"
#include "Components/AudioComponent.h"
#include "Spaceship/Selector/SkinOptionsDataAsset.h"
#include "Spaceship/UAnalysisDataManager.h"
#include "Spaceship/SpaceshipSaveManager.h"
#include "AVisualizerActor.generated.h"

USTRUCT(BlueprintType)
struct FThresholdResult
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Threshold")
	FTransform Transform;

	UPROPERTY(BlueprintReadWrite, Category = "Threshold")
	float Speed;
};
USTRUCT(BlueprintType)
struct FBandAnalysisData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category="Audio")
	TArray<float> BandValues;
};

USTRUCT(BlueprintType)
struct FPreAnalyzedAudioData
{
	GENERATED_BODY()

	// Durée totale du son analysé
	UPROPERTY(BlueprintReadWrite, Category="Audio")
	float Duration = 0.0f;

	// Tableau des valeurs pré-analysées par bande
	UPROPERTY(BlueprintReadWrite, Category="Audio")
	TArray<FBandAnalysisData> BandValuesOverTime;
};

/**
 * Acteur qui génère et gère un visualiseur audio.
 */
UCLASS()
class SPACESHIP_API AVisualizerActor : public AActor
{
	GENERATED_BODY()

public:
	AVisualizerActor();
	virtual void BeginPlay() override;

	// Référence au DataAsset contenant les paramètres
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualizer")
	UVisualizerManager* VisualizerSettings;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualizer")
	bool bIsColorWhite = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualizer")
	int32 ColorChangeThreshold = 5;

	// Limites de déplacement pour le visualiseur
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualizer")
	FVector MovementBoundsMin = FVector(-500.0f, -500.0f, -500.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualizer")
	FVector MovementBoundsMax = FVector(500.0f, 500.0f, 500.0f);

	//BossClass ref
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualizer")
	TSubclassOf<AEnemyBoss> BossClass;

	UPROPERTY(EditAnywhere, Category = "Visualizer")
	float OscillationSpeed = 5.0f; // Vitesse indépendante de la musique

	//MaxDisplacement
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualizer")
	float MaxDisplacement = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Materials")
	FMaterialSet ColorMaterials;
	// In public section:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Audio")
	UAudioComponent* AudioComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualizer")
	USkinOptionsDataAsset* SkinOptions;
	/**
	 * FUNCTIONS
	 */
	// Initialisation du visualiseur
	UFUNCTION(BlueprintCallable, Category = "Visualizer")
	void InitializeVisualizer();
	
	UFUNCTION(BlueprintCallable, Category = "Visualizer")
	void UpdateVisualizerAtTime(const float InSeconds);
	// Mise à jour du visualiseur
	UFUNCTION(BlueprintCallable, Category = "Visualizer")
	void UpdateVisualizer(const TArray<float>& BandValues);

	// In public UFUNCTION section:
	UFUNCTION(BlueprintCallable, Category = "Audio")
	UAudioComponent* GetAudioComponent() const { return AudioComponent; }
	// Réinitialisation du visualiseur
	UFUNCTION(BlueprintCallable, Category = "Visualizer")
	void ResetVisualizer();

	UFUNCTION(BlueprintCallable, Category = "Visualizer")
	void ResetAndRespawnVisualizer();
	
	UFUNCTION(BlueprintCallable, Category = "Visualizer")
	TArray<float> CalculateBandAverages(const TArray<float>& BandValues, int32 NumSamplesPerBand);

	UFUNCTION(BlueprintCallable, Category = "Visualizer")
	void BeginCalculTest();

	UFUNCTION(BlueprintCallable, Category = "Visualizer")
	void CalculateThresholds(const TArray<float>& BandAverages);
	
	UFUNCTION(BlueprintCallable, Category = "Visualizer")
	void CheckThresholds( const TArray<float>& BandValues);

	UFUNCTION(BlueprintCallable, Category = "Visualizer")
	float CalculateGlobalAmplitude(const TArray<float>& BandValues);

	UFUNCTION(BlueprintCallable, Category = "Visualizer")
	void SpawnBoss();
	
	UFUNCTION(BlueprintCallable, Category = "Visualizer")
	void SpawnSoundSphereFromThreshold( int32 BandIndex, float SpeedMultiplier);
	UPROPERTY(BlueprintReadOnly, Category = "Audio")
	float MusicDuration;

	// Sound wave currently being analyzed
	UPROPERTY(BlueprintReadOnly, Category = "Audio")
	USoundWave* CurrentSoundWave;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Analysis")
	UAnalysisDataManager* AnalysisDataManager;
protected:
	// Tableau des barres générées
	UPROPERTY()
	TArray<AActor*> SpawnedBars;
	UPROPERTY()
	AObjectPoolManager* PoolManager;
	
	int32 LowFrequencyExceedCount = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualizer")
	float SpawnCooldown = 0.5f; // Temps de cooldown entre deux spawns pour chaque bande

	UPROPERTY()
	TArray<float> BandCooldownTimers; // Stocke le temps restant avant qu'une bande puisse réactiver une barre

	// Vitesse du mouvement basé sur l'amplitude sonore
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualizer")
	float MovementSpeed = 50.0f;
	
	// Axe d'échelle des barres
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualizer")
	EScaleAxis DisplacementAxis = EScaleAxis::Z; // Par défaut, échelle sur Z.
	
	// Save manager reference
	UPROPERTY()
	USpaceshipSaveManager* SaveManager;

	// Timer handle for updates
	FTimerHandle UpdateTimerHandle;

	
private:
	FTimerHandle BossSlideTimerHandle; // Timer pour le mouvement du boss
	FVector LinearPosition; // Nouvelle variable pour suivre la position de base
	int32 MovementDirection = 1;
	float CurrentMovementSpeed = 0.0f;
	void StartBossSlide(const FVector& TargetLocation);
	bool bFirstTime = true;
	//garder une ref au boss
	UPROPERTY()
	AEnemyBoss* Boss;
	// Accumulateur de temps pour gérer l'oscillation
	float TimeAccumulator = 0.0f;
	// Offset courant du visualiseur
	FVector CurrentOffset = FVector::ZeroVector;// Méthode pour générer les barres
	void SpawnBars();
	void CheckLowFrequenciesAndChangeColor(const TArray<float>& BandValues);
	void SpawnBarAtLocation(const FVector& Location, const FRotator& Rotation);
	void SpawnCircularBars(const int32 Bands, const float TotalAngle);
	void SpawnAlignedBars(const int32 Bands);

	// Initialize audio analysis settings
	void InitializeAudioAnalysis();
};
