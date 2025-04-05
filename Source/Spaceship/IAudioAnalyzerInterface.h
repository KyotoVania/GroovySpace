// IAudioAnalyzerInterface.h
#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IAudioAnalyzerInterface.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class UAudioAnalyzerInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Interface for audio analysis classes.
 * This interface defines the methods that any audio analyzer class should implement.
 */
class SPACESHIP_API IAudioAnalyzerInterface
{
	GENERATED_BODY()

public:
	virtual void Initialize(class USoundWave* InSound) = 0;
	virtual void GetFrequencyData(float InSeconds, TArray<float>& OutFrequencies) = 0;
	virtual void GetBandData(float InSeconds, TArray<float>& OutBands) = 0;
	virtual void CalculateThresholds(float DifficultyMultiplier) = 0;
	virtual const TArray<float>& GetThresholds() const = 0;
	virtual float GetSoundDuration() const = 0;
	virtual int32 GetNumBands() const = 0;
};