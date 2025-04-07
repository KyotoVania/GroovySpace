#include "UAnalysisDataManager.h"

// Implementation
UConstantQNRT* UAnalysisDataManager::FindAnalysisDataForSound(const USoundWave* InSoundWave) const
{
	if (!InSoundWave)
	{
		return nullptr;
	}

	for (const TObjectPtr<UConstantQNRT>& AnalysisInstance : PreAnalyzedData)
	{
		if (AnalysisInstance && AnalysisInstance->Sound == InSoundWave)
		{
			return AnalysisInstance;
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("Analysis data not found for SoundWave: %s"), *InSoundWave->GetName());
	return nullptr;
}