// BossMovementComponent.cpp
#include "BossMovementComponent.h"
// Include directly instead of using Public/ path which might not be correct
#include "UVisualizerManager.h"

UBossMovementComponent::UBossMovementComponent()
    : MovementAxis(EScaleAxis::X)
    , MovementSpeed(50.0f)
    , OscillationSpeed(5.0f)
    , MaxDisplacement(100.0f)
    , MovementBoundsMin(FVector(-500.0f, -500.0f, -500.0f))
    , MovementBoundsMax(FVector(500.0f, 500.0f, 500.0f))
    , MovementDirection(1)
    , TimeAccumulator(0.0f)
    , bIsSliding(false)
    , SlideDuration(2.0f)
    , SlideTimeAccumulator(0.0f)
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UBossMovementComponent::BeginPlay()
{
    Super::BeginPlay();
    
    // Initialize linear position to actor's current location
    LinearPosition = GetOwner()->GetActorLocation();
    
    // Apply bounds to ensure we're in a valid position
    LinearPosition = ApplyBounds(LinearPosition);
}

void UBossMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    
    // Handle sliding animation
    if (bIsSliding)
    {
        SlideTimeAccumulator += DeltaTime;
        
        // Calculate progress (0.0 to 1.0)
        float Alpha = FMath::Clamp(SlideTimeAccumulator / SlideDuration, 0.0f, 1.0f);
        
        // Interpolate position
        FVector NewLocation = FMath::Lerp(SlideStartLocation, SlideTargetLocation, Alpha);
        GetOwner()->SetActorLocation(NewLocation);
        
        // End sliding when complete
        if (Alpha >= 1.0f)
        {
            bIsSliding = false;
            LinearPosition = SlideTargetLocation;
        }
        
        return; // Skip regular movement during slide
    }
    
    // Increment oscillation time
    TimeAccumulator += DeltaTime * OscillationSpeed;
    
    // Calculate oscillation
    float Oscillation = FMath::Sin(TimeAccumulator) * MaxDisplacement;
    
    // Apply to appropriate axis
    FVector OscillationOffset = FVector::ZeroVector;
    
    // Use if-else instead of switch for enum
    if (MovementAxis == EScaleAxis::X)
    {
        OscillationOffset.X = Oscillation;
    }
    else if (MovementAxis == EScaleAxis::Y) 
    {
        OscillationOffset.Y = Oscillation;
    }
    else if (MovementAxis == EScaleAxis::Z)
    {
        OscillationOffset.Z = Oscillation;
    }
    
    // Combine base position and oscillation
    FVector FinalPosition = LinearPosition + OscillationOffset;
    
    // Apply bounds
    FinalPosition = ApplyBounds(FinalPosition);
    
    // Update actor position
    GetOwner()->SetActorLocation(FinalPosition);
}

void UBossMovementComponent::StartSlide(const FVector& TargetLocation, float Duration)
{
    // Initialize slide parameters
    bIsSliding = true;
    SlideStartLocation = GetOwner()->GetActorLocation();
    SlideTargetLocation = TargetLocation;
    SlideDuration = FMath::Max(Duration, 0.1f); // Prevent division by zero
    SlideTimeAccumulator = 0.0f;
}

void UBossMovementComponent::UpdatePositionWithAmplitude(float GlobalAmplitude)
{
    // Skip if currently sliding
    if (bIsSliding)
    {
        return;
    }
    
    // Scale movement speed by global amplitude
    float AdjustedSpeed = MovementSpeed * (1.0f + GlobalAmplitude);
    
    // Calculate movement delta
    FVector MovementDelta = FVector::ZeroVector;
    float Delta = AdjustedSpeed * GetWorld()->GetDeltaSeconds() * MovementDirection;
    
    // Use if-else instead of switch for enum
    if (MovementAxis == EScaleAxis::X)
    {
        MovementDelta.X = Delta;
    }
    else if (MovementAxis == EScaleAxis::Y)
    {
        MovementDelta.Y = Delta;
    }
    else if (MovementAxis == EScaleAxis::Z)
    {
        MovementDelta.Z = Delta;
    }
    
    // Update linear position
    LinearPosition += MovementDelta;
    
    // Check bounds and reverse direction if needed
    bool bShouldReverse = false;
    
    // Use if-else instead of switch for enum
    if (MovementAxis == EScaleAxis::X)
    {
        if (LinearPosition.X > MovementBoundsMax.X || LinearPosition.X < MovementBoundsMin.X)
            bShouldReverse = true;
    }
    else if (MovementAxis == EScaleAxis::Y)
    {
        if (LinearPosition.Y > MovementBoundsMax.Y || LinearPosition.Y < MovementBoundsMin.Y)
            bShouldReverse = true;
    }
    else if (MovementAxis == EScaleAxis::Z)
    {
        if (LinearPosition.Z > MovementBoundsMax.Z || LinearPosition.Z < MovementBoundsMin.Z)
            bShouldReverse = true;
    }
    
    if (bShouldReverse)
    {
        MovementDirection *= -1;
        LinearPosition = ApplyBounds(LinearPosition);
    }
}

void UBossMovementComponent::SetMovementBounds(const FVector& Min, const FVector& Max)
{
    MovementBoundsMin = Min;
    MovementBoundsMax = Max;
    
    // Ensure current position is within bounds
    LinearPosition = ApplyBounds(LinearPosition);
}

FVector UBossMovementComponent::ApplyBounds(const FVector& Position)
{
    return Position.BoundToBox(MovementBoundsMin, MovementBoundsMax);
}