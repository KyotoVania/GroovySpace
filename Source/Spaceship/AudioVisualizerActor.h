// AudioVisualizerActor.h
/**
 * Visualizer actor that integrates all audio visualization components
 * 
 * This class serves as a central integration point for the modular audio visualization system.
 * It manages:
 * - Audio analysis through Synesthesia
 * - Bar visualization (through AudioVisualizerComponent)
 * - Boss spawning and movement (through BossMovementComponent)
 * - Sound sphere spawning (through SoundSphereSpawnerComponent)
 * 
 * Usage:
 * 1. Set desired audio, bar actor class, and boss class in editor
 * 2. Call InitializeAudio with a SoundWave
 * 3. System will automatically visualize audio, spawn projectiles, and move the boss
 */
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "IAudioAnalyzerInterface.h"
#include "AudioVisualizerComponent.h"
#include "BossMovementComponent.h"
#include "SoundSphereSpawnerComponent.h"
#include "AEnemyBoss.h"
#include "AObjectPoolManager.h"
#include "AudioVisualizerActor.generated.h"

UCLASS()
class SPACESHIP_API AAudioVisualizerActor : public AActor
{
    GENERATED_BODY()

public:
    AAudioVisualizerActor();
    
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    
    // Initialize the audio analyzer
    UFUNCTION(BlueprintCallable, Category = "Audio Visualization")
    void InitializeAudio(USoundWave* InSound);
    
    // Initialize the visualizer
    UFUNCTION(BlueprintCallable, Category = "Audio Visualization")
    void InitializeVisualizer();
    
    // Spawn the boss
    UFUNCTION(BlueprintCallable, Category = "Boss")
    void SpawnBoss();
    
    // Update at a specific time
    UFUNCTION(BlueprintCallable, Category = "Audio Visualization")
    void UpdateAtTime(float InSeconds);
    
    // Reset the visualizer
    UFUNCTION(BlueprintCallable, Category = "Audio Visualization")
    void ResetVisualizer();

    // Sound property getters and setters
    UFUNCTION(BlueprintCallable, Category = "Audio")
    USoundWave* GetSound() const;
    
    UFUNCTION(BlueprintCallable, Category = "Audio")
    void SetSound(USoundWave* InSound);
    
    // Set difficulty
    UFUNCTION(BlueprintCallable, Category = "Audio Visualization")
    void SetDifficulty(float Difficulty);

public:
    // Audio manager
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
    TObjectPtr<class UAudioAnalysisManager> AudioManager;
    
    // Pool manager for sound spheres
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
    TObjectPtr<AObjectPoolManager> SoundSpherePool;
    
    // Boss class
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss")
    TSubclassOf<AEnemyBoss> BossClass;
    
    // Materials for different colors
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization")
    FMaterialSet ColorMaterials;
    
    // Difficulty multiplier
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
    float DifficultyMultiplier;
    
    // Auto spawn boss on beginplay
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss")
    bool bAutoSpawnBoss;
    
    // Visualizer component
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UAudioVisualizerComponent> VisualizerComponent;
    
    // Boss movement component
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UBossMovementComponent> BossMovementComponent;
    
    // Sound sphere spawner component
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<USoundSphereSpawnerComponent> SphereSpawnerComponent;

private:
    // Current audio playback time
    float CurrentTime;
    
    // Playback speed
    float PlaybackSpeed;
    
    // Is playback active?
    bool bIsPlaying;
    
    // Current boss instance
    UPROPERTY()
    TObjectPtr<AEnemyBoss> Boss;
    
    // Audio analyzer interface
    TScriptInterface<IAudioAnalyzerInterface> AudioAnalyzer;
};
