#include "ASegmentManager.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

ASegmentManager::ASegmentManager()
{
    PrimaryActorTick.bCanEverTick = true; // Activer Tick
}

void ASegmentManager::BeginPlay()
{
    Super::BeginPlay();

    // Récupérer la longueur des segments
    if (Segments.Num() > 0 && Segments[0])
    {
        SegmentLength = Segments[0]->GetSegmentSize().X;
    }

    // Placer les segments dans leur position initiale
    for (int32 i = 0; i < Segments.Num(); ++i)
    {
        if (Segments[i])
        {
            FVector InitialPosition = LoopStartPosition + FVector(i * SegmentLength, 0.0f, 0.0f);
            Segments[i]->SetActorLocation(InitialPosition);
        }
    }
}

void ASegmentManager::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    MoveSegments(DeltaTime);
}

void ASegmentManager::MoveSegments(float DeltaTime)
{
    // Déplacement des segments
    FVector Movement = FVector(-ScrollSpeed * DeltaTime, 0.0f, 0.0f);

    for (ABatSegment* Segment : Segments)
    {
        if (Segment)
        {
            Segment->AddActorWorldOffset(Movement);

            // Si le segment est hors de la vue de la caméra, on le replace
            if (Segment->GetActorLocation().X < LoopEndPosition.X)
            {
                LoopSegment(Segment);
            }
        }
    }
}

void ASegmentManager::LoopSegment(ABatSegment* Segment)
{
    //on reposionne le segment en X pour le faire réapparaitre au debut du loop
    FVector NewLocation = FVector(LoopStartPosition.X + SegmentLength, 0.0f, 0.0f);
    Segment->SetActorLocation(NewLocation);
}
