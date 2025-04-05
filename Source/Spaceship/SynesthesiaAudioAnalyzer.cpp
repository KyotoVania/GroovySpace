
// SynesthesiaAudioAnalyzer.cpp
#include "SynesthesiaAudioAnalyzer.h"
#include "UObject/UnrealType.h"

USynesthesiaAudioAnalyzer::USynesthesiaAudioAnalyzer()
    : ConstantQNRT(nullptr)
    , bThresholdsCalculated(false)
{
}

void USynesthesiaAudioAnalyzer::Initialize(USoundWave* InSound)
{
    if (!ConstantQNRT)
    {
        ConstantQNRT = NewObject<UConstantQNRT>(this);
        ConstantQNRT->Settings = NewObject<UConstantQNRTSettings>(ConstantQNRT);
        ConstantQNRT->Settings->NumBands = 32; // Default, can be adjusted
        ConstantQNRT->Settings->StartingFrequency = 20.0f; // 20Hz
    }
    
    if (InSound)
    {
        ConstantQNRT->Sound = InSound;
        ForceReanalyze();
        
        // Reset thresholds when changing sounds
        bThresholdsCalculated = false;
        BandThresholds.Empty();
        BandAverages.Empty();
    }
}

void USynesthesiaAudioAnalyzer::GetFrequencyData(float InSeconds, TArray<float>& OutFrequencies)
{
    if (ConstantQNRT && ConstantQNRT->Sound)
    {
        ConstantQNRT->GetNormalizedChannelConstantQAtTime(InSeconds, 0, OutFrequencies);
    }
    else
    {
        OutFrequencies.Empty();
    }
}

void USynesthesiaAudioAnalyzer::GetBandData(float InSeconds, TArray<float>& OutBands)
{
    if (ConstantQNRT && ConstantQNRT->Sound)
    {
        ConstantQNRT->GetNormalizedChannelConstantQAtTime(InSeconds, 0, OutBands);
    }
    else
    {
        OutBands.Empty();
    }
}

void USynesthesiaAudioAnalyzer::CalculateThresholds(float DifficultyMultiplier)
{
    if (!ConstantQNRT || !ConstantQNRT->Sound || !ConstantQNRT->Settings)
    {
        UE_LOG(LogTemp, Warning, TEXT("CalculateThresholds: Invalid ConstantQNRT, Sound, or Settings"));
        return;
    }
    
    // Calculate band averages if not already done
    if (BandAverages.Num() == 0)
    {
        CalculateBandAverages();
    }
    
    const int32 NumBands = ConstantQNRT->Settings->NumBands;
    
    // Apply difficulty multiplier to thresholds
    BandThresholds.Empty(NumBands);
    for (int32 BandIndex = 0; BandIndex < NumBands; ++BandIndex)
    {
        if (BandAverages.IsValidIndex(BandIndex))
        {
            float Threshold = BandAverages[BandIndex] * DifficultyMultiplier;
            BandThresholds.Add(Threshold);
        }
        else
        {
            BandThresholds.Add(0.1f); // Default fallback
        }
    }
    
    bThresholdsCalculated = true;
    
    // Debug output
    for (int32 BandIndex = 0; BandIndex < BandThresholds.Num(); ++BandIndex)
    {
        UE_LOG(LogTemp, Log, TEXT("Band %d Threshold: %f"), BandIndex, BandThresholds[BandIndex]);
    }
}

const TArray<float>& USynesthesiaAudioAnalyzer::GetThresholds() const
{
    return BandThresholds;
}

float USynesthesiaAudioAnalyzer::GetSoundDuration() const
{
    if (ConstantQNRT && ConstantQNRT->Sound)
    {
        return ConstantQNRT->Sound->Duration;
    }
    return 0.0f;
}

int32 USynesthesiaAudioAnalyzer::GetNumBands() const
{
    if (ConstantQNRT && ConstantQNRT->Settings)
    {
        return ConstantQNRT->Settings->NumBands;
    }
    return 0;
}

void USynesthesiaAudioAnalyzer::SetConstantQNRT(UConstantQNRT* InConstantQNRT)
{
    ConstantQNRT = InConstantQNRT;
    
    // Reset thresholds when changing analyzers
    bThresholdsCalculated = false;
    BandThresholds.Empty();
    BandAverages.Empty();
}

void USynesthesiaAudioAnalyzer::ForceReanalyze()
{
    if (!ConstantQNRT || !ConstantQNRT->Settings || !ConstantQNRT->Sound)
    {
        UE_LOG(LogTemp, Warning, TEXT("ForceReanalyze: Invalid ConstantQNRT, Settings, or Sound"));
        return;
    }
    
    // Simulate a property change to force reanalysis
    UE_LOG(LogTemp, Log, TEXT("ForceReanalyze: Triggering analysis recalculation for sound: %s"), 
           *ConstantQNRT->Sound->GetName());

    // Make a small change to force reanalysis
    ConstantQNRT->Settings->StartingFrequency += 0.001f;
    ConstantQNRT->Settings->StartingFrequency -= 0.001f;

    // Mark as modified to ensure reanalysis
    ConstantQNRT->Modify();
    ConstantQNRT->MarkPackageDirty();
}

void USynesthesiaAudioAnalyzer::CalculateBandAverages()
{
    if (!ConstantQNRT || !ConstantQNRT->Sound || !ConstantQNRT->Settings)
    {
        UE_LOG(LogTemp, Warning, TEXT("CalculateBandAverages: Invalid ConstantQNRT, Sound, or Settings"));
        return;
    }
    
    const float Duration = ConstantQNRT->Sound->Duration;
    const int32 NumBands = ConstantQNRT->Settings->NumBands;
    
    if (Duration <= 0.0f || NumBands <= 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("CalculateBandAverages: Invalid sound duration or number of bands"));
        return;
    }
    
    // Initialize band averages array
    BandAverages.Init(0.0f, NumBands);
    
    // Sample the song at regular intervals
    const float AnalysisStep = 0.1f; // Sample every 100ms
    TArray<float> CurrentBandValues;
    int32 SampleCount = 0;
    
    for (float CurrentTime = 0.0f; CurrentTime < Duration; CurrentTime += AnalysisStep)
    {
        ConstantQNRT->GetNormalizedChannelConstantQAtTime(CurrentTime, 0, CurrentBandValues);
        
        if (CurrentBandValues.Num() != NumBands)
        {
            UE_LOG(LogTemp, Warning, TEXT("Band value count mismatch. Expected %d, got %d"), 
                  NumBands, CurrentBandValues.Num());
            continue;
        }
        
        // Accumulate values
        for (int32 BandIndex = 0; BandIndex < NumBands; ++BandIndex)
        {
            BandAverages[BandIndex] += CurrentBandValues[BandIndex];
        }
        
        SampleCount++;
    }
    
    // Calculate averages
    if (SampleCount > 0)
    {
        for (int32 BandIndex = 0; BandIndex < NumBands; ++BandIndex)
        {
            BandAverages[BandIndex] /= SampleCount;
        }
    }
    
    // Debug output
    for (int32 BandIndex = 0; BandIndex < BandAverages.Num(); ++BandIndex)
    {
        UE_LOG(LogTemp, Log, TEXT("Band %d Average: %f"), BandIndex, BandAverages[BandIndex]);
    }
}