#pragma once

#include "CoreMinimal.h"
#include "AudioSynesthesia/Classes/ConstantQNRT.h"
#include "Sound/SoundWave.h"
#include "DynamicQNRTManager.generated.h"

/**
 * Simple wrapper to set and get the Sound property of UConstantQNRT
 */
UCLASS(Blueprintable)
class SPACESHIP_API USoundPropertyManager : public UObject
{
	GENERATED_BODY()

public:
	/** Set the Sound property of a UConstantQNRT object */
	UFUNCTION(BlueprintCallable, Category = "Audio")
	static void SetSound(UConstantQNRT* ConstantQNRT, USoundWave* InSound);

	/** Get the Sound property of a UConstantQNRT object */
	UFUNCTION(BlueprintCallable, Category = "Audio")
	static USoundWave* GetSound(UConstantQNRT* ConstantQNRT);

	UFUNCTION(BlueprintCallable, Category = "Audio")
	static void ForceReanalyze(UConstantQNRT* ConstantQNRT);

};
