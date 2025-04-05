// SynesthesiaAudioAnalyzer.h
#pragma once

#include "CoreMinimal.h"
#include "IAudioAnalyzerInterface.h"
#include "AudioSynesthesia/Classes/ConstantQNRT.h"
#include "Sound/SoundWave.h"
#include "SynesthesiaAudioAnalyzer.generated.h"

/**
 * Audio analyzer implementation using Unreal's Synesthesia system
 */
UCLASS(Blueprintable)
class SPACESHIP_API USynesthesiaAudioAnalyzer : public UObject, public IAudioAnalyzerInterface
{
    GENERATED_BODY()

public:
    USynesthesiaAudioAnalyzer();

    // IAudioAnalyzer interface
    virtual void Initialize(USoundWave* InSound) override;
    virtual void GetFrequencyData(float InSeconds, TArray<float>& OutFrequencies) override;
    virtual void GetBandData(float InSeconds, TArray<float>& OutBands) override;
    virtual void CalculateThresholds(float DifficultyMultiplier) override;
    virtual const TArray<float>& GetThresholds() const override;
    virtual float GetSoundDuration() const override;
    virtual int32 GetNumBands() const override;
    
    // Synesthesia-specific methods
    UFUNCTION(BlueprintCallable, Category = "Audio Analysis")
    void SetConstantQNRT(UConstantQNRT* InConstantQNRT);
    
    UFUNCTION(BlueprintCallable, Category = "Audio Analysis")
    UConstantQNRT* GetConstantQNRT() const { return ConstantQNRT; }
    
    // Force Synesthesia to reanalyze the sound
    UFUNCTION(BlueprintCallable, Category = "Audio Analysis")
    void ForceReanalyze();

private:
    // Synesthesia analyzer
    UPROPERTY()
    UConstantQNRT* ConstantQNRT;
    
    // Cached thresholds for each band
    UPROPERTY()
    TArray<float> BandThresholds;
    
    // Flag to track if thresholds have been calculated
    bool bThresholdsCalculated;
    
    // Cached band averages used for threshold calculation
    TArray<float> BandAverages;
    
    // Calculate the average intensity for each frequency band across the duration of the sound
    void CalculateBandAverages();
};
