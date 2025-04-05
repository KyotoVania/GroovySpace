
// AudioAnalysisManager.cpp
#include "AudioAnalysisManager.h"
#include "SynesthesiaAudioAnalyzer.h"

UAudioAnalysisManager::UAudioAnalysisManager()
    : CurrentSound(nullptr)
{
}

TScriptInterface<IAudioAnalyzerInterface> UAudioAnalysisManager::GetAnalyzer()
{
    if (!CurrentAnalyzer.GetObject())
    {
        // Create Synesthesia analyzer
        UObject* AnalyzerObject = NewObject<USynesthesiaAudioAnalyzer>(this);
        
        CurrentAnalyzer.SetObject(AnalyzerObject);
        CurrentAnalyzer.SetInterface(Cast<IAudioAnalyzerInterface>(AnalyzerObject));
        
        // If we have a sound, initialize the analyzer with it
        if (CurrentSound && CurrentAnalyzer.GetInterface())
        {
            CurrentAnalyzer->Initialize(CurrentSound);
        }
    }
    
    return CurrentAnalyzer;
}

void UAudioAnalysisManager::SetSound(USoundWave* InSound)
{
    CurrentSound = InSound;
    
    // Initialize the current analyzer with the new sound
    if (CurrentSound && CurrentAnalyzer.GetInterface())
    {
        CurrentAnalyzer->Initialize(CurrentSound);
    }
}

void UAudioAnalysisManager::GetFrequencyData(float InSeconds, TArray<float>& OutFrequencies)
{
    if (CurrentAnalyzer.GetInterface())
    {
        CurrentAnalyzer->GetFrequencyData(InSeconds, OutFrequencies);
    }
    else
    {
        OutFrequencies.Empty();
    }
}

void UAudioAnalysisManager::GetBandData(float InSeconds, TArray<float>& OutBands)
{
    if (CurrentAnalyzer.GetInterface())
    {
        CurrentAnalyzer->GetBandData(InSeconds, OutBands);
    }
    else
    {
        OutBands.Empty();
    }
}

void UAudioAnalysisManager::CalculateThresholds(float DifficultyMultiplier)
{
    if (CurrentAnalyzer.GetInterface())
    {
        CurrentAnalyzer->CalculateThresholds(DifficultyMultiplier);
    }
}

TArray<float> UAudioAnalysisManager::GetThresholds() const
{
    if (CurrentAnalyzer.GetInterface())
    {
        return CurrentAnalyzer->GetThresholds();
    }
    
    return TArray<float>();
}

float UAudioAnalysisManager::GetSoundDuration() const
{
    if (CurrentAnalyzer.GetInterface())
    {
        return CurrentAnalyzer->GetSoundDuration();
    }
    
    return 0.0f;
}

int32 UAudioAnalysisManager::GetNumBands() const
{
    if (CurrentAnalyzer.GetInterface())
    {
        return CurrentAnalyzer->GetNumBands();
    }
    
    return 0;
}