
// AudioVisualizerActor.cpp
#include "AudioVisualizerActor.h"
#include "AudioAnalysisManager.h"
#include "Components/StaticMeshComponent.h"

AAudioVisualizerActor::AAudioVisualizerActor()
    : AudioManager(nullptr)
    , SoundSpherePool(nullptr)
    , BossClass(nullptr)
    , DifficultyMultiplier(1.5f)
    , bAutoSpawnBoss(true)
    , CurrentTime(0.0f)
    , PlaybackSpeed(1.0f)
    , bIsPlaying(false)
    , Boss(nullptr)
{
    PrimaryActorTick.bCanEverTick = true;
    
    // Create root component
    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    
    // Create components
    VisualizerComponent = CreateDefaultSubobject<UAudioVisualizerComponent>(TEXT("VisualizerComponent"));
    BossMovementComponent = CreateDefaultSubobject<UBossMovementComponent>(TEXT("BossMovementComponent"));
    SphereSpawnerComponent = CreateDefaultSubobject<USoundSphereSpawnerComponent>(TEXT("SphereSpawnerComponent"));
}

void AAudioVisualizerActor::BeginPlay()
{
    Super::BeginPlay();
    
    // Create audio manager if not set
    if (!AudioManager)
    {
        AudioManager = NewObject<UAudioAnalysisManager>(this);
    }
    
    // Initialize components
    InitializeVisualizer();
    
    // Auto spawn boss
    if (bAutoSpawnBoss)
    {
        SpawnBoss();
    }
}

void AAudioVisualizerActor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    
    if (bIsPlaying && AudioAnalyzer.GetInterface())
    {
        // Update playback time
        CurrentTime += DeltaTime * PlaybackSpeed;
        
        // Loop if we reach the end
        float Duration = AudioAnalyzer->GetSoundDuration();
        if (CurrentTime >= Duration)
        {
            CurrentTime = 0.0f;
        }
        
        // Update visualization at current time
        UpdateAtTime(CurrentTime);
    }
}

void AAudioVisualizerActor::InitializeAudio(USoundWave* InSound)
{
    if (!AudioManager)
    {
        AudioManager = NewObject<UAudioAnalysisManager>(this);
    }
    
    // Set sound and get analyzer
    AudioManager->SetSound(InSound);
    AudioAnalyzer = AudioManager->GetAnalyzer();
    
    // Calculate thresholds
    if (AudioAnalyzer.GetInterface())
    {
        AudioAnalyzer->CalculateThresholds(DifficultyMultiplier);
    }
    
    // Reset playback
    CurrentTime = 0.0f;
    bIsPlaying = true;
    
    // Reinitialize visualizer
    InitializeVisualizer();
}

void AAudioVisualizerActor::InitializeVisualizer()
{
    // Configure visualizer component
    if (VisualizerComponent)
    {
        VisualizerComponent->ColorMaterials = ColorMaterials;
        
        if (AudioAnalyzer.GetInterface())
        {
            VisualizerComponent->Initialize(AudioAnalyzer);
        }
    }
    
    // Configure sphere spawner
    if (SphereSpawnerComponent)
    {
        SphereSpawnerComponent->ColorMaterials = ColorMaterials;
        
        if (AudioAnalyzer.GetInterface() && VisualizerComponent && SoundSpherePool)
        {
            SphereSpawnerComponent->Initialize(AudioAnalyzer, VisualizerComponent, SoundSpherePool);
        }
    }
}

void AAudioVisualizerActor::SpawnBoss()
{
    if (!BossClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("SpawnBoss: No boss class set"));
        return;
    }
    
    // Destroy existing boss
    if (Boss)
    {
        Boss->Destroy();
        Boss = nullptr;
    }
    
    // Spawn position
    FVector SpawnLocation = GetActorLocation() + FVector(0.0f, 0.0f, 200.0f);
    FRotator SpawnRotation = GetActorRotation();
    
    // Spawn boss
    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = this;
    
    Boss = GetWorld()->SpawnActor<AEnemyBoss>(BossClass, SpawnLocation, SpawnRotation, SpawnParams);
    
    if (Boss)
    {
        // Attach to this actor
        Boss->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
        
        // Start slide animation
        if (BossMovementComponent)
        {
            FVector TargetLocation = GetActorLocation() + FVector(0.0f, 0.0f, 200.0f);
            BossMovementComponent->StartSlide(TargetLocation, 2.0f);
        }
    }
}

void AAudioVisualizerActor::UpdateAtTime(float InSeconds)
{
    if (!AudioAnalyzer.GetInterface())
    {
        return;
    }
    
    // Get frequency data at current time
    TArray<float> BandValues;
    AudioAnalyzer->GetBandData(InSeconds, BandValues);
    
    // Update visualizer
    if (VisualizerComponent)
    {
        VisualizerComponent->UpdateVisualizer(BandValues);
    }
    
    // Update sphere spawner
    if (SphereSpawnerComponent)
    {
        SphereSpawnerComponent->Update(BandValues);
    }
    
    // Update boss movement
    if (BossMovementComponent && !BossMovementComponent->IsSliding())
    {
        float GlobalAmplitude = VisualizerComponent->CalculateGlobalAmplitude(BandValues);
        BossMovementComponent->UpdatePositionWithAmplitude(GlobalAmplitude);
    }
}

void AAudioVisualizerActor::ResetVisualizer()
{
    if (VisualizerComponent)
    {
        VisualizerComponent->ResetVisualizer();
    }
    
    // Respawn visualizer
    InitializeVisualizer();
}

USoundWave* AAudioVisualizerActor::GetSound() const
{
    if (AudioManager)
    {
        return AudioManager->GetSound();
    }
    
    return nullptr;
}

void AAudioVisualizerActor::SetSound(USoundWave* InSound)
{
    InitializeAudio(InSound);
}

void AAudioVisualizerActor::SetDifficulty(float Difficulty)
{
    DifficultyMultiplier = FMath::Max(Difficulty, 1.0f);
    
    if (AudioAnalyzer.GetInterface())
    {
        AudioAnalyzer->CalculateThresholds(DifficultyMultiplier);
    }
}