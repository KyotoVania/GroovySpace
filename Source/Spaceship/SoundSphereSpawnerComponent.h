// SoundSphereSpawnerComponent.h
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "IAudioAnalyzerInterface.h"
#include "AudioVisualizerComponent.h"
#include "AObjectPoolManager.h"
#include "FMaterialSet.h"
#include "SoundSphereSpawnerComponent.generated.h"

/**
 * Component that spawns sound spheres based on audio analysis
 */
UCLASS(ClassGroup=(Audio), meta=(BlueprintSpawnableComponent))
class SPACESHIP_API USoundSphereSpawnerComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    USoundSphereSpawnerComponent();
    
    virtual void BeginPlay() override;
    
    // Initialize the component
    UFUNCTION(BlueprintCallable, Category = "Sound Sphere")
    void Initialize(TScriptInterface<IAudioAnalyzerInterface> InAnalyzer, 
                    UAudioVisualizerComponent* InVisualizer,
                    AObjectPoolManager* InPoolManager);
    
    // Update based on latest audio data
    UFUNCTION(BlueprintCallable, Category = "Sound Sphere")
    void Update(const TArray<float>& BandValues);
    
    // Spawn a sound sphere from a specific visualizer bar
    UFUNCTION(BlueprintCallable, Category = "Sound Sphere")
    void SpawnSoundSphere(int32 BarIndex, float SpeedMultiplier);
    
    // Set current color
    UFUNCTION(BlueprintCallable, Category = "Sound Sphere")
    void SetColor(bool bIsWhite);

public:
    // Cooldown between sphere spawns for each band
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound Sphere")
    float SpawnCooldown;
    
    // Materials for different colors
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound Sphere")
    FMaterialSet ColorMaterials;

private:
    // Audio analyzer reference
    TScriptInterface<IAudioAnalyzerInterface> AudioAnalyzer;
    
    // Visualizer component reference
    UPROPERTY()
    UAudioVisualizerComponent* VisualizerComponent;
    
    // Object pool manager reference
    UPROPERTY()
    AObjectPoolManager* PoolManager;
    
    // Current color state
    bool bIsColorWhite;
    
    // Cooldown timers for each band
    TArray<float> BandCooldownTimers;
};
