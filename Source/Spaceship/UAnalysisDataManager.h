#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "AudioSynesthesia/Classes/ConstantQNRT.h"
#include "Sound/SoundWave.h"
#include "UAnalysisDataManager.generated.h"

UCLASS()
class SPACESHIP_API UAnalysisDataManager : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	// Array of pre-analyzed ConstantQNRT instances
	UPROPERTY(EditAnywhere, Instanced, BlueprintReadOnly, Category = "Analysis Data")
	TArray<TObjectPtr<UConstantQNRT>> PreAnalyzedData;

	// Helper function to find analysis data for a given SoundWave
	UFUNCTION(BlueprintPure, Category = "Analysis Data")
	UConstantQNRT* FindAnalysisDataForSound(const USoundWave* InSoundWave) const;
};
