
// AudioVisualizerComponent.cpp
#include "AudioVisualizerComponent.h"

UAudioVisualizerComponent::UAudioVisualizerComponent()
    : BarActorClass(nullptr)
    , VisualizerShape(EVisualizerShape::Aligned)
    , BandSpacing(50.0f)
    , BarRotation(FRotator(0.0f, 90.0f, 0.0f))
    , AlignmentAxis(FVector(0.0f, 1.0f, 0.0f))
    , ScaleAxis(EScaleAxis::Z)
    , ScaleMultiplier(50.0f)
    , ColorChangeThreshold(5)
    , bIsColorWhite(true)
    , LowFrequencyExceedCount(0)
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UAudioVisualizerComponent::BeginPlay()
{
    Super::BeginPlay();
}

void UAudioVisualizerComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    ResetVisualizer();
    Super::EndPlay(EndPlayReason);
}

void UAudioVisualizerComponent::Initialize(TScriptInterface<IAudioAnalyzerInterface> InAudioAnalyzer)
{
    AudioAnalyzer = InAudioAnalyzer;
    SpawnBars();
}

void UAudioVisualizerComponent::UpdateVisualizer(const TArray<float>& BandValues)
{
    if (BandValues.Num() != SpawnedBars.Num() || SpawnedBars.Num() == 0)
    {
        return;
    }
    
    // Check low frequencies for potential color change
    if (CheckLowFrequencies(BandValues, AudioAnalyzer->GetThresholds()))
    {
        // Change color
        bIsColorWhite = !bIsColorWhite;
        SetBarColors(bIsColorWhite);
        
        // Notify about color change
        OnColorChanged.Broadcast(bIsColorWhite);
    }
    
    // Update bar scales based on band values
    for (int32 i = 0; i < SpawnedBars.Num(); ++i)
    {
        AActor* Bar = SpawnedBars[i];
        if (Bar)
        {
            FVector NewScale(1.0f, 1.0f, 1.0f);
            float ScaleValue = BandValues[i] * ScaleMultiplier;
            
            switch (ScaleAxis)
            {
                case EScaleAxis::X:
                    NewScale.X = ScaleValue;
                    break;
                case EScaleAxis::Y:
                    NewScale.Y = ScaleValue;
                    break;
                case EScaleAxis::Z:
                    NewScale.Z = ScaleValue;
                    break;
            }
            
            Bar->SetActorScale3D(NewScale);
        }
    }
}

void UAudioVisualizerComponent::ResetVisualizer()
{
    for (AActor* Bar : SpawnedBars)
    {
        if (Bar)
        {
            Bar->Destroy();
        }
    }
    
    SpawnedBars.Empty();
    LowFrequencyExceedCount = 0;
}

void UAudioVisualizerComponent::SpawnBars()
{
    // Clean up existing bars
    ResetVisualizer();
    
    if (!AudioAnalyzer.GetInterface())
    {
        UE_LOG(LogTemp, Warning, TEXT("SpawnBars: No audio analyzer set"));
        return;
    }
    
    const int32 Bands = AudioAnalyzer->GetNumBands();
    if (Bands <= 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("SpawnBars: Invalid number of bands"));
        return;
    }
    
    switch (VisualizerShape)
    {
        case EVisualizerShape::Aligned:
            SpawnAlignedBars(Bands);
            break;
        case EVisualizerShape::Circle:
            SpawnCircularBars(Bands, 360.0f);
            break;
        case EVisualizerShape::HalfCircle:
            SpawnCircularBars(Bands, 180.0f);
            break;
        case EVisualizerShape::CustomCircle:
            SpawnCircularBars(Bands, 150.0f);
            break;
        default:
            UE_LOG(LogTemp, Warning, TEXT("SpawnBars: Unknown visualizer shape"));
            break;
    }
    
    // Initialize all bars with the current color
    SetBarColors(bIsColorWhite);
}

bool UAudioVisualizerComponent::CheckLowFrequencies(const TArray<float>& BandValues, const TArray<float>& Thresholds)
{
    if (BandValues.Num() == 0 || Thresholds.Num() == 0 || BandValues.Num() != Thresholds.Num())
    {
        return false;
    }
    
    // Check the first few bands (low frequencies)
    const int32 LowFrequencyStart = 0;
    const int32 LowFrequencyEnd = FMath::Min(3, BandValues.Num() - 1);
    
    bool bExceededThreshold = false;
    
    for (int32 i = LowFrequencyStart; i <= LowFrequencyEnd; ++i)
    {
        if (BandValues[i] > Thresholds[i])
        {
            bExceededThreshold = true;
            break;
        }
    }
    
    if (bExceededThreshold)
    {
        LowFrequencyExceedCount++;
        
        if (LowFrequencyExceedCount >= ColorChangeThreshold)
        {
            LowFrequencyExceedCount = 0;
            return true; // Trigger color change
        }
    }
    
    return false;
}

float UAudioVisualizerComponent::CalculateGlobalAmplitude(const TArray<float>& BandValues)
{
    if (BandValues.Num() == 0)
    {
        return 0.0f;
    }
    
    float Sum = 0.0f;
    for (float Value : BandValues)
    {
        Sum += Value;
    }
    
    return Sum / BandValues.Num();
}

void UAudioVisualizerComponent::SetVisualizerShape(EVisualizerShape NewShape)
{
    if (VisualizerShape != NewShape)
    {
        VisualizerShape = NewShape;
        SpawnBars();
    }
}

void UAudioVisualizerComponent::SetBandSpacing(float NewSpacing)
{
    if (!FMath::IsNearlyEqual(BandSpacing, NewSpacing))
    {
        BandSpacing = NewSpacing;
        SpawnBars();
    }
}

void UAudioVisualizerComponent::SetBarColors(bool bWhite)
{
    bIsColorWhite = bWhite;
    
    UMaterialInterface* Material = bWhite ? 
        ColorMaterials.WhiteMaterial : 
        ColorMaterials.BlackMaterial;
    
    if (Material)
    {
        for (AActor* Bar : SpawnedBars)
        {
            if (Bar)
            {
                UStaticMeshComponent* Mesh = Bar->FindComponentByClass<UStaticMeshComponent>();
                if (Mesh)
                {
                    Mesh->SetMaterial(0, Material);
                }
            }
        }
    }
}

void UAudioVisualizerComponent::SpawnAlignedBars(int32 Bands)
{
    const FVector AlignedAxis = AlignmentAxis.GetSafeNormal();
    
    for (int32 i = 0; i < Bands; ++i)
    {
        const FVector Location = GetOwner()->GetActorLocation() + (AlignedAxis * i * BandSpacing);
        SpawnBarAtLocation(Location, BarRotation);
    }
}

void UAudioVisualizerComponent::SpawnCircularBars(int32 Bands, float TotalAngle)
{
    const float Radius = BandSpacing * Bands / (2 * PI);
    const FVector Center = GetOwner()->GetActorLocation();
    const float AngleStep = FMath::DegreesToRadians(TotalAngle / Bands);
    
    // Calculate perpendicular axis
    FVector PerpendicularAxis = FVector::CrossProduct(AlignmentAxis, FVector::UpVector);
    if (PerpendicularAxis.IsNearlyZero())
    {
        PerpendicularAxis = FVector::CrossProduct(AlignmentAxis, FVector::ForwardVector);
    }
    PerpendicularAxis.Normalize();
    
    for (int32 i = 0; i < Bands; ++i)
    {
        const float Angle = AngleStep * i;
        
        // Generate position in a circle/semi-circle
        FVector LocalPosition = 
            FMath::Cos(Angle) * Radius * AlignmentAxis +
            FMath::Sin(Angle) * Radius * PerpendicularAxis;
        
        // Calculate world position
        FVector FinalPosition = Center + LocalPosition;
        
        // Calculate rotation to face center
        FRotator Rotation = FRotationMatrix::MakeFromX(-LocalPosition).Rotator();
        
        SpawnBarAtLocation(FinalPosition, Rotation);
    }
}

AActor* UAudioVisualizerComponent::SpawnBarAtLocation(const FVector& Location, const FRotator& Rotation)
{
    if (!BarActorClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("SpawnBarAtLocation: No bar actor class set"));
        return nullptr;
    }
    
    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = GetOwner();
    
    AActor* Bar = GetWorld()->SpawnActor<AActor>(
        BarActorClass,
        Location,
        Rotation,
        SpawnParams
    );
    
    if (Bar)
    {
        Bar->AttachToActor(GetOwner(), FAttachmentTransformRules::KeepWorldTransform);
        SpawnedBars.Add(Bar);
    }
    
    return Bar;
}