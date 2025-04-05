// AudioAnalysisManager.h
#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "IAudioAnalyzerInterface.h"
#include "Sound/SoundWave.h"
#include "AudioAnalysisManager.generated.h"

/**
 * Manager for audio analysis using Synesthesia
 */
UCLASS(Blueprintable, BlueprintType)
class SPACESHIP_API UAudioAnalysisManager : public UObject
{
    GENERATED_BODY()

public:
    UAudioAnalysisManager();
    
    // Get or create the audio analyzer
    UFUNCTION(BlueprintCallable, Category = "Audio Analysis")
    TScriptInterface<IAudioAnalyzerInterface> GetAnalyzer();
    
    // Set the sound to analyze
    UFUNCTION(BlueprintCallable, Category = "Audio Analysis")
    void SetSound(USoundWave* InSound);
    
    // Get the current sound
    UFUNCTION(BlueprintCallable, Category = "Audio Analysis")
    USoundWave* GetSound() const { return CurrentSound; }
    
    // Get frequency data at a specific time
    UFUNCTION(BlueprintCallable, Category = "Audio Analysis")
    void GetFrequencyData(float InSeconds, TArray<float>& OutFrequencies);
    
    // Get band data at a specific time
    UFUNCTION(BlueprintCallable, Category = "Audio Analysis")
    void GetBandData(float InSeconds, TArray<float>& OutBands);
    
    // Calculate thresholds for frequency bands
    UFUNCTION(BlueprintCallable, Category = "Audio Analysis")
    void CalculateThresholds(float DifficultyMultiplier = 1.0f);
    
    // Get the current thresholds
    UFUNCTION(BlueprintCallable, Category = "Audio Analysis")
    TArray<float> GetThresholds() const;
    
    // Get the duration of the current sound
    UFUNCTION(BlueprintCallable, Category = "Audio Analysis")
    float GetSoundDuration() const;
    
    // Get the number of frequency bands
    UFUNCTION(BlueprintCallable, Category = "Audio Analysis")
    int32 GetNumBands() const;
    
private:
    // Current sound being analyzed
    UPROPERTY()
    USoundWave* CurrentSound;
    
    // Current analyzer
    UPROPERTY()
    TScriptInterface<IAudioAnalyzerInterface> CurrentAnalyzer;
};
