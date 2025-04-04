#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ABatSegment.generated.h"

UCLASS()
class SPACESHIP_API ABatSegment : public AActor
{
	GENERATED_BODY()

public:
	ABatSegment();

	UFUNCTION(BlueprintCallable, Category = "Segment")
	FVector GetSegmentSize() const;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Segment")
	TArray<AActor*> BuildingMeshes;
protected:
	virtual void BeginPlay() override;

private:
	FVector CachedSize;
};