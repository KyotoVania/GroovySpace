// BatSegment.cpp
#include "ABatSegment.h"

ABatSegment::ABatSegment()
{
	PrimaryActorTick.bCanEverTick = false;
}

FVector ABatSegment::GetSegmentSize() const
{
	return CachedSize;
}

void ABatSegment::BeginPlay()
{
	Super::BeginPlay();

	// Pré-calcul de la taille
	FBox Bounds(ForceInit);
	for (AActor* Building : BuildingMeshes)
	{
		if (Building)
		{
			// Log avant attachement
			UE_LOG(LogTemp, Log, TEXT("Before Attach: %s - World Transform: %s"), *Building->GetName(), *Building->GetActorTransform().ToString());

			// Conserver la transformation locale lors de l'attachement
			FTransform RelativeTransform = Building->GetActorTransform().GetRelativeTransform(GetActorTransform());
			Building->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepWorldTransform);
			Building->SetActorRelativeTransform(RelativeTransform);

			// Log après attachement
			UE_LOG(LogTemp, Log, TEXT("After Attach: %s - Relative Transform: %s"), *Building->GetName(), *Building->GetActorTransform().ToString());

			Bounds += Building->GetComponentsBoundingBox();
		}
	}
	CachedSize = Bounds.GetSize();
}
