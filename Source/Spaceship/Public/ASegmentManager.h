#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ABatSegment.h"
#include "ASegmentManager.generated.h"
UENUM(BlueprintType)
enum class EScrollAxis : uint8
{
	X,
	Y,
	Z
};

UCLASS()
class SPACESHIP_API ASegmentManager : public AActor
{
	GENERATED_BODY()

public:
	ASegmentManager();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Segments")
	float ScrollSpeed = 500.f; // Vitesse du défilement

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Segments")
	float SegmentLength = 1500.f; // Longueur fixe des 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Segments")
	FVector LoopStartPosition = FVector(0.0f, 0.0f, 0.0f); // Position de début du loop

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Segments")
	FVector LoopEndPosition = FVector(4500.0f, 0.0f, 0.0f); // Position de fin du loop

	// Trois segments définis directement dans l'éditeur
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Segments")
	TArray<ABatSegment*> Segments;

	virtual void Tick(float DeltaTime) override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Segments")
	EScrollAxis ScrollAxis = EScrollAxis::X;
protected:
	virtual void BeginPlay() override;

private:
	void MoveSegments(float DeltaTime);
	void LoopSegment(ABatSegment* Segment);
};
