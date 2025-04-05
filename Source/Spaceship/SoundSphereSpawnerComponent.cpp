
// SoundSphereSpawnerComponent.cpp
#include "SoundSphereSpawnerComponent.h"
#include "ASoundSphere.h"
#include "Kismet/GameplayStatics.h"

USoundSphereSpawnerComponent::USoundSphereSpawnerComponent()
    : SpawnCooldown(0.5f)
    , VisualizerComponent(nullptr)
    , PoolManager(nullptr)
    , bIsColorWhite(true)
{
    PrimaryComponentTick.bCanEverTick = false;
}

void USoundSphereSpawnerComponent::BeginPlay()
{
    Super::BeginPlay();
}

void USoundSphereSpawnerComponent::Initialize(TScriptInterface<IAudioAnalyzerInterface> InAnalyzer, 
                                            UAudioVisualizerComponent* InVisualizer,
                                            AObjectPoolManager* InPoolManager)
{
    AudioAnalyzer = InAnalyzer;
    VisualizerComponent = InVisualizer;
    PoolManager = InPoolManager;
    
    if (AudioAnalyzer.GetInterface())
    {
        // Initialize cooldown timers for each band
        const int32 NumBands = AudioAnalyzer->GetNumBands();
        BandCooldownTimers.Init(0.0f, NumBands);
    }
    
    // Listen for color changes from visualizer
    if (VisualizerComponent)
    {
        VisualizerComponent->OnColorChanged.AddDynamic(this, &USoundSphereSpawnerComponent::SetColor);
        bIsColorWhite = VisualizerComponent->IsColorWhite();
    }
}

void USoundSphereSpawnerComponent::Update(const TArray<float>& BandValues)
{
    if (!AudioAnalyzer.GetInterface() || !VisualizerComponent || !PoolManager)
    {
        return;
    }
    
    const TArray<float>& Thresholds = AudioAnalyzer->GetThresholds();
    
    if (BandValues.Num() != Thresholds.Num() || BandValues.Num() != BandCooldownTimers.Num())
    {
        return;
    }
    
    // Update cooldown timers
    for (int32 i = 0; i < BandCooldownTimers.Num(); ++i)
    {
        if (BandCooldownTimers[i] > 0.0f)
        {
            BandCooldownTimers[i] -= GetWorld()->GetDeltaSeconds();
        }
        
        // Check if band exceeds threshold and cooldown has elapsed
        if (BandValues[i] > Thresholds[i] && BandCooldownTimers[i] <= 0.0f)
        {
            // Calculate speed multiplier based on how much the threshold was exceeded
            float SpeedMultiplier = BandValues[i] - Thresholds[i];
            
            // Spawn sound sphere
            SpawnSoundSphere(i, SpeedMultiplier);
            
            // Reset cooldown for this band
            BandCooldownTimers[i] = SpawnCooldown;
        }
    }
}

void USoundSphereSpawnerComponent::SpawnSoundSphere(int32 BarIndex, float SpeedMultiplier)
{
    if (!PoolManager || !VisualizerComponent)
    {
        return;
    }
    
    // Get sound sphere from pool
    ASoundSphere* SoundSphere = Cast<ASoundSphere>(PoolManager->GetPooledObject());
    if (!SoundSphere)
    {
        UE_LOG(LogTemp, Warning, TEXT("SpawnSoundSphere: Failed to get sound sphere from pool"));
        return;
    }
    
    // Get position and rotation from visualizer bar
    if (VisualizerComponent->SpawnedBars.IsValidIndex(BarIndex))
    {
        AActor* Bar = VisualizerComponent->SpawnedBars[BarIndex];
        if (Bar)
        {
            FVector SpawnLocation = Bar->GetActorLocation();
            FRotator BarRotation = Bar->GetActorRotation();
            
            // Calculate direction based on bar rotation
            float Yaw = BarRotation.Yaw;
            FVector Direction = FVector(
                FMath::Cos(FMath::DegreesToRadians(Yaw)),
                FMath::Sin(FMath::DegreesToRadians(Yaw)),
                0.0f
            );
            
            // Configure sound sphere
            SoundSphere->SetActorLocation(SpawnLocation);
            SoundSphere->SetDirection(Direction);
            SoundSphere->SetSpeed(1.0f + SpeedMultiplier);
            
            // Set color
            UMaterialInterface* Material = bIsColorWhite ? 
                ColorMaterials.WhiteMaterial : 
                ColorMaterials.BlackMaterial;
            
            SoundSphere->SetMaterialColor(Material);
            SoundSphere->SetColor(bIsColorWhite);
        }
    }
}

void USoundSphereSpawnerComponent::SetColor(bool bIsWhite)
{
    bIsColorWhite = bIsWhite;
}