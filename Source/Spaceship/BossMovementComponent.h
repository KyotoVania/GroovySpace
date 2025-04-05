// BossMovementComponent.h
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "UVisualizerManager.h" // Include for EScaleAxis enum
#include "BossMovementComponent.generated.h"

/**
 * Controls boss movement based on audio input
 */
UCLASS(ClassGroup=(Audio), meta=(BlueprintSpawnableComponent))
class SPACESHIP_API UBossMovementComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UBossMovementComponent();
    
    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
    
    // Start sliding the boss to a target location
    UFUNCTION(BlueprintCallable, Category = "Boss")
    void StartSlide(const FVector& TargetLocation, float Duration = 2.0f);
    
    // Update boss position based on audio amplitude
    UFUNCTION(BlueprintCallable, Category = "Boss")
    void UpdatePositionWithAmplitude(float GlobalAmplitude);
    
    // Set movement bounds
    UFUNCTION(BlueprintCallable, Category = "Boss")
    void SetMovementBounds(const FVector& Min, const FVector& Max);
    
    // Is the boss currently sliding?
    UFUNCTION(BlueprintCallable, Category = "Boss")
    bool IsSliding() const { return bIsSliding; }

public:
    // Primary movement axis
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    EScaleAxis MovementAxis;
    
    // Base movement speed
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float MovementSpeed;
    
    // Oscillation speed
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float OscillationSpeed;
    
    // Maximum oscillation displacement
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float MaxDisplacement;
    
    // Movement bounds
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    FVector MovementBoundsMin;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    FVector MovementBoundsMax;

private:
    // Linear position (base position without oscillation)
    FVector LinearPosition;
    
    // Current movement direction (1 or -1)
    int32 MovementDirection;
    
    // Time accumulator for oscillation
    float TimeAccumulator;
    
    // Slide parameters
    bool bIsSliding;
    FVector SlideStartLocation;
    FVector SlideTargetLocation;
    float SlideDuration;
    float SlideTimeAccumulator;
    
    // Apply bounds to a position
    FVector ApplyBounds(const FVector& Position);
};