// AudioVisualizerComponent.h
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "IAudioAnalyzerInterface.h"
#include "FMaterialSet.h"
#include "UVisualizerManager.h" // For EVisualizerShape and other enums
#include "AudioVisualizerComponent.generated.h"

// Declare delegate for color change events
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnColorChanged, bool, bIsWhite);

/**
 * Component that visualizes audio data by spawning and manipulating bar actors
 */
UCLASS(ClassGroup=(Audio), meta=(BlueprintSpawnableComponent))
class SPACESHIP_API UAudioVisualizerComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UAudioVisualizerComponent();
    
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    
    // Initialize the visualizer
    UFUNCTION(BlueprintCallable, Category = "Audio Visualization")
    void Initialize(TScriptInterface<IAudioAnalyzerInterface> InAudioAnalyzer);
    
    // Update the visualizer with the latest audio data
    UFUNCTION(BlueprintCallable, Category = "Audio Visualization")
    void UpdateVisualizer(const TArray<float>& BandValues);
    
    // Reset the visualizer
    UFUNCTION(BlueprintCallable, Category = "Audio Visualization")
    void ResetVisualizer();
    
    // Spawn new bars based on current settings
    UFUNCTION(BlueprintCallable, Category = "Audio Visualization")
    void SpawnBars();
    
    // Check if any low frequencies exceed their thresholds
    UFUNCTION(BlueprintCallable, Category = "Audio Visualization")
    bool CheckLowFrequencies(const TArray<float>& BandValues, const TArray<float>& Thresholds);
    
    // Calculate the global amplitude from band values
    UFUNCTION(BlueprintCallable, Category = "Audio Visualization")
    float CalculateGlobalAmplitude(const TArray<float>& BandValues);
    
    // Get the current color state
    UFUNCTION(BlueprintCallable, Category = "Audio Visualization")
    bool IsColorWhite() const { return bIsColorWhite; }
    
    // Set the visualizer shape
    UFUNCTION(BlueprintCallable, Category = "Audio Visualization")
    void SetVisualizerShape(EVisualizerShape NewShape);
    
    // Get the visualizer shape
    UFUNCTION(BlueprintCallable, Category = "Audio Visualization")
    EVisualizerShape GetVisualizerShape() const { return VisualizerShape; }
    
    // Set band spacing
    UFUNCTION(BlueprintCallable, Category = "Audio Visualization")
    void SetBandSpacing(float NewSpacing);
    
    // Change the color of all bars
    UFUNCTION(BlueprintCallable, Category = "Audio Visualization")
    void SetBarColors(bool bWhite);

    // Callback for when color changes
    UPROPERTY(BlueprintAssignable, Category = "Audio Visualization")
    FOnColorChanged OnColorChanged;

public:
    // Class of actor to spawn for bars
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization")
    TSubclassOf<AActor> BarActorClass;
    
    // Shape of the visualizer
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization")
    EVisualizerShape VisualizerShape;
    
    // Spacing between bands
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization")
    float BandSpacing;
    
    // Bar rotation
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization")
    FRotator BarRotation;
    
    // Alignment axis
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization")
    FVector AlignmentAxis;
    
    // Scale axis
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization")
    EScaleAxis ScaleAxis;
    
    // Scale multiplier
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization")
    float ScaleMultiplier;
    
    // Materials for different colors
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization")
    FMaterialSet ColorMaterials;
    
    // Threshold for color change
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization")
    int32 ColorChangeThreshold;
    
    // Spawned bar actors (accessible for projectile spawning)
    UPROPERTY(BlueprintReadOnly, Category = "Visualization")
    TArray<AActor*> SpawnedBars;
    
private:
    // Audio analyzer
    TScriptInterface<IAudioAnalyzerInterface> AudioAnalyzer;
    
    // Current color state
    bool bIsColorWhite;
    
    // Counter for low frequency threshold breaches
    int32 LowFrequencyExceedCount;
    
    // Spawn bars in an aligned pattern
    void SpawnAlignedBars(int32 Bands);
    
    // Spawn bars in a circular pattern
    void SpawnCircularBars(int32 Bands, float TotalAngle);
    
    // Spawn a bar at the specified location and rotation
    AActor* SpawnBarAtLocation(const FVector& Location, const FRotator& Rotation);
};
