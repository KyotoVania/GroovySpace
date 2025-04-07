#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "UVisualizerManager.h"
#include "SongOptionsDataAsset.generated.h"

USTRUCT(BlueprintType)
struct FSongMetadata
{
	GENERATED_BODY()

	// Display name for the song
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Song")
	FString DisplayName;

	// The actual sound wave asset
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Song")
	USoundWave* SoundWave;

	// Recommended difficulty (1-10)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Song", meta = (ClampMin = "1", ClampMax = "10"))
	int32 RecommendedDifficulty = 5;

	// Preview start time (in seconds)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Preview")
	float PreviewStartTime = 0.0f;

	// Is this song locked initially?
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Song")
	bool bLockedByDefault = false;
};

UCLASS(BlueprintType)
class SPACESHIP_API USongOptionsDataAsset : public UDataAsset
{
	GENERATED_BODY()
    
public:
	// List of all available songs with metadata
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Songs")
	TArray<FSongMetadata> AvailableSongs;
	
};