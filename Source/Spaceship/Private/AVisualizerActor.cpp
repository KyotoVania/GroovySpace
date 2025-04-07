#include "AVisualizerActor.h"

#include "EngineUtils.h"
#include "Engine/World.h"

#include "Kismet/GameplayStatics.h"

AVisualizerActor::AVisualizerActor()
{
	PrimaryActorTick.bCanEverTick = true;


	// Create audio component
	AudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));
	AudioComponent->bAutoActivate = false;
	AudioComponent->SetupAttachment(RootComponent);
}

void AVisualizerActor::BeginPlay()
{
	Super::BeginPlay();

	// Get save manager
	SaveManager = USpaceshipSaveManager::GetSaveManager(GetWorld());
    
	if (!VisualizerSettings)
	{
		UE_LOG(LogTemp, Error, TEXT("VisualizerSettings is NULL! Cannot initialize visualizer."));
		return;
	}

	// Update difficulty from save game
	if (SaveManager && SaveManager->CurrentSave)
	{
		// Set materials from SkinOptions based on saved skin ID
		if (SkinOptions && SkinOptions->AvailableSkins.IsValidIndex(SaveManager->CurrentSave->ColorSkinID))
		{
			ColorMaterials = SkinOptions->AvailableSkins[SaveManager->CurrentSave->ColorSkinID].ProjectileMaterialSets;
		}
		else if (SkinOptions)
		{
			// Fallback to default materials
			ColorMaterials.WhiteMaterial = SkinOptions->DefaultWhiteMaterial;
			ColorMaterials.BlackMaterial = SkinOptions->DefaultBlackMaterial;
		}
		else
		{
			// Fallback to default materials
			ColorMaterials.WhiteMaterial = nullptr;
			ColorMaterials.BlackMaterial = nullptr;
		}
		// Get the last played song
		USoundWave* LastSong = SaveManager->CurrentSave->LastSong.Get();
		if (LastSong && AnalysisDataManager)
		{
			// Find pre-analyzed data
			UConstantQNRT* PreAnalyzedData = AnalysisDataManager->FindAnalysisDataForSound(LastSong);
			if (PreAnalyzedData)
			{
				// Use the pre-analyzed data
				VisualizerSettings->ConstantQNRT = PreAnalyzedData;
				CurrentSoundWave = LastSong;
				AudioComponent->SetSound(CurrentSoundWave);
            
				// No need to call BeginCalculTest() as data is pre-analyzed
				// Just initialize visualizer components
				InitializeAudioAnalysis();
				InitializeVisualizer();
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Pre-analyzed data not found for song: %s"), *LastSong->GetName());
			}
		}
	}
	for (TActorIterator<AObjectPoolManager> It(GetWorld()); It; ++It)
	{
		PoolManager = *It;
		break;
	}
	// Initialize audio analysis

	
	// Store initial position and apply bounds
	LinearPosition = GetActorLocation();
	LinearPosition = LinearPosition.BoundToBox(MovementBoundsMin, MovementBoundsMax);
}

void AVisualizerActor::InitializeAudioAnalysis()
{	
	if (!VisualizerSettings || !VisualizerSettings->ConstantQNRT)
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid VisualizerSettings or ConstantQNRT"));
		return;
	}

	auto* QNRT = VisualizerSettings->ConstantQNRT;
    
	// Ensure settings exist
	if (!QNRT->Settings)
	{
		QNRT->Settings = NewObject<UConstantQNRTSettings>(QNRT);
		if (!QNRT->Settings)
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to create ConstantQNRT Settings"));
			return;
		}
	}

	// Configure default settings if needed
	if (QNRT->Settings->NumBands <= 0)
	{
		QNRT->Settings->NumBands = 32;  // Default number of frequency bands
		QNRT->Settings->StartingFrequency = 20.0f;  // Low audible frequency
	}

	// Initialize cooldown timers
	BandCooldownTimers.Init(0.0f, QNRT->Settings->NumBands);

	// Begin calculation of thresholds if sound is set
	if (QNRT->Sound)
	{
		CurrentSoundWave = QNRT->Sound;
		BeginCalculTest();
	}
}
void AVisualizerActor::InitializeVisualizer()
{
	if (!VisualizerSettings)
	{
		UE_LOG(LogTemp, Error, TEXT("VisualizerSettings is NULL! Cannot initialize visualizer."));
		return;
	}

	if (!VisualizerSettings->ConstantQNRT || !VisualizerSettings->ConstantQNRT->Settings)
	{
		UE_LOG(LogTemp, Error, TEXT("ConstantQNRT or Settings is NULL!"));
		return;
	}

	const int32 Bands = VisualizerSettings->ConstantQNRT->Settings->NumBands;
	if (Bands <= 0)
	{
		UE_LOG(LogTemp, Error, TEXT("VisualizerSettings->ConstantQNRT->Settings->NumBands is invalid (<= 0)"));
		return;
	}

	BandCooldownTimers.Init(0.0f, Bands);
	// Générer les barres
	SpawnBars();
	SpawnBoss();
}



void AVisualizerActor::SpawnBars()
{
	if (!VisualizerSettings || !VisualizerSettings->ConstantQNRT || !VisualizerSettings->ConstantQNRT->Settings)
	{
		UE_LOG(LogTemp, Warning, TEXT("VisualizerSettings, ConstantQNRT, or Settings is null"));
		return;
	}
	const int32 Bands = VisualizerSettings->ConstantQNRT->Settings->NumBands;

	switch (VisualizerSettings->VisualizerShape)
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
		UE_LOG(LogTemp, Warning, TEXT("Unknown Visualizer Shape"));
		break;
	}
}

void AVisualizerActor::SpawnBoss()
{
	if (BossClass)
	{
		// Position de départ (au-dessus du visualiseur)
		FVector SpawnLocation;
		if (bFirstTime == true)
		{
			SpawnLocation = GetActorLocation() + FVector(0.0f, 0.0f, 200.0f); // Ajuster Z pour le spawn en haut
			bFirstTime = false;
		}
		else
		{
			SpawnLocation = GetActorLocation() + FVector(0.0f, 0.0f, 2000.0f); // Ajuster Z pour le spawn en haut
		}
		FRotator SpawnRotation = GetActorRotation();

		// Créer le boss
		Boss = GetWorld()->SpawnActor<AEnemyBoss>(BossClass, SpawnLocation, SpawnRotation);
		if (Boss)
		{
			Boss->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);

			// Lancer un mouvement interpolé pour descendre jusqu'à la position cible
			FVector TargetLocation = GetActorLocation() + FVector(0.0f, 0.0f, 200.0f); // Position finale
			StartBossSlide(TargetLocation);
		}
	}
}

void AVisualizerActor::StartBossSlide(const FVector& TargetLocation)
{
    if (!Boss)
    {
        return;
    }

    // Reset the timer and the accumulated time to avoid errors between spawns
    GetWorld()->GetTimerManager().ClearTimer(BossSlideTimerHandle);
    TimeAccumulator = 0.0f;  // Reset the accumulated time

    // Define values for the animation of the movement
    const float SlideDuration = 2.0f;
    const FVector StartLocation = Boss->GetActorLocation();

    GetWorld()->GetTimerManager().SetTimer(BossSlideTimerHandle, [this, StartLocation, TargetLocation, SlideDuration]() mutable {
        // Update the percentage of progress
        float Alpha = TimeAccumulator / SlideDuration;
        Alpha = FMath::Clamp(Alpha, 0.0f, 1.0f); // Prevent exceeding 100%

        FVector NewLocation = FMath::Lerp(StartLocation, TargetLocation, Alpha);

        if (Boss)
        {
            Boss->SetActorLocation(NewLocation);
        }

        // Increment the accumulated time
        TimeAccumulator += GetWorld()->GetDeltaSeconds();

        // Check if the animation is finished
        if (TimeAccumulator >= SlideDuration)
        {
            GetWorld()->GetTimerManager().ClearTimer(BossSlideTimerHandle);
            UE_LOG(LogTemp, Log, TEXT("Boss reached target location."));
        }

    }, GetWorld()->GetDeltaSeconds(), true);
}



void AVisualizerActor::SpawnAlignedBars(const int32 Bands)
{
	const FVector AlignmentAxis = VisualizerSettings->AlignmentAxis.GetSafeNormal();
	const float Spacing = VisualizerSettings->BandSpacing;

	for (int32 i = 0; i < Bands; ++i)
	{
		const FVector Location = GetActorLocation() + (AlignmentAxis * i * Spacing);
		SpawnBarAtLocation(Location, VisualizerSettings->BarRotation);
	}
}

void AVisualizerActor::SpawnCircularBars(const int32 Bands, const float TotalAngle)
{
	const float Radius = VisualizerSettings->BandSpacing * Bands / (2 * PI); // Rayon approximatif
	const FVector Center = GetActorLocation(); // Centre du cercle
	const float AngleStep = TotalAngle / Bands;

	// Calculer un vecteur perpendiculaire à l'axe d'alignement
	FVector PerpendicularAxis = FVector::CrossProduct(VisualizerSettings->AlignmentAxis, FVector::UpVector);
	if (PerpendicularAxis.IsNearlyZero()) // Si l'axe est parallèle à UpVector, utiliser une autre référence
	{
		PerpendicularAxis = FVector::CrossProduct(VisualizerSettings->AlignmentAxis, FVector::ForwardVector);
	}
	PerpendicularAxis.Normalize();

	for (int32 i = 0; i < Bands; ++i)
	{
		// Calculer l'angle pour chaque barre
		const float Angle = FMath::DegreesToRadians(AngleStep * i);

		// Générer les coordonnées locales dans un cercle/demi-cercle
		FVector LocalPosition = 
			FMath::Cos(Angle) * Radius * VisualizerSettings->AlignmentAxis +  // Aligner sur l'axe principal
			FMath::Sin(Angle) * Radius * PerpendicularAxis;                   // Ajouter la composante perpendiculaire

		// Calculer la position globale
		FVector FinalPosition = Center + LocalPosition;

		// Calculer la rotation pour aligner chaque barre avec le cercle
		FRotator Rotation = FRotationMatrix::MakeFromX(LocalPosition).Rotator();

		// Spawner la barre à la position et rotation calculées
		SpawnBarAtLocation(FinalPosition, Rotation);
	}
}



void AVisualizerActor::SpawnBarAtLocation(const FVector& Location, const FRotator& Rotation)
{
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;

	AActor* Bar = GetWorld()->SpawnActor<AActor>(
		VisualizerSettings->BarActorClass,
		Location,
		Rotation,
		SpawnParams
	);

	if (Bar)
	{
		// Attach the bar to the visualizer actor
		Bar->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
		SpawnedBars.Add(Bar);
	}
}

void AVisualizerActor::UpdateVisualizer(const TArray<float>& BandValues)
{
	if (BandValues.Num() != SpawnedBars.Num()) return;

	// 1. Calcul du mouvement linéaire de base
	float GlobalAmplitude = 1.0f+ CalculateGlobalAmplitude(BandValues);
	float CurrentSpeed = MovementSpeed * GlobalAmplitude;
	// Mise à jour direction linéaire
	FVector LinearDelta = FVector::ZeroVector;
	switch (DisplacementAxis)
	{
	case EScaleAxis::X: 
		LinearDelta.X = CurrentSpeed * GetWorld()->GetDeltaSeconds() * MovementDirection;
		break;
	case EScaleAxis::Y: 
		LinearDelta.Y = CurrentSpeed * GetWorld()->GetDeltaSeconds() * MovementDirection;
		break;
	case EScaleAxis::Z: 
		LinearDelta.Z = CurrentSpeed * GetWorld()->GetDeltaSeconds() * MovementDirection;
		break;
	}
	LinearPosition += LinearDelta;

	// 2. Gestion des rebonds pour l'axe principal
	bool bShouldReverse = false;
	switch (DisplacementAxis)
	{
	case EScaleAxis::X:
		if (LinearPosition.X > MovementBoundsMax.X || LinearPosition.X < MovementBoundsMin.X) 
			bShouldReverse = true;
		break;
	case EScaleAxis::Y:
		if (LinearPosition.Y > MovementBoundsMax.Y || LinearPosition.Y < MovementBoundsMin.Y) 
			bShouldReverse = true;
		break;
	case EScaleAxis::Z:
		if (LinearPosition.Z > MovementBoundsMax.Z || LinearPosition.Z < MovementBoundsMin.Z) 
			bShouldReverse = true;
		break;
	}

	if (bShouldReverse)
	{
		MovementDirection *= -1;
		LinearPosition = LinearPosition.BoundToBox(MovementBoundsMin, MovementBoundsMax);
	}

	// 3. Oscillation sinusoïdale sur axe secondaire
	TimeAccumulator += GetWorld()->GetDeltaSeconds() * OscillationSpeed;
	FVector SineOffset = FVector::ZeroVector;
	float Oscillation = FMath::Sin(TimeAccumulator) * MaxDisplacement * GlobalAmplitude;
	switch (DisplacementAxis)
	{
	case EScaleAxis::X: SineOffset.X = Oscillation; break;
	case EScaleAxis::Y: SineOffset.Y = Oscillation; break;
	case EScaleAxis::Z: SineOffset.Z = Oscillation; break;
	}

	// 4. Combinaison des deux mouvements
	FVector FinalPosition = LinearPosition + SineOffset;
	FinalPosition = FinalPosition.BoundToBox(MovementBoundsMin, MovementBoundsMax); // Clamping final

	SetActorLocation(FinalPosition);	
	for (int32 i = 0; i < SpawnedBars.Num(); ++i)
	{
		AActor* Bar = SpawnedBars[i];
		if (Bar)
		{
			
			FVector NewScale(1.0f, 1.0f, 1.0f); // Par défaut, ne rien changer.
			float ScaleValue = BandValues[i] * 50.0f; // Exemple de calcul d'échelle.
			// Appliquer l'échelle sur l'axe spécifié
			switch (VisualizerSettings->ScaleAxis)
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
			// Mettre à jour l'échelle de l'acteur
			Bar->SetActorScale3D(NewScale);
		}
	}

	CheckLowFrequenciesAndChangeColor(BandValues);
}


void AVisualizerActor::ResetVisualizer()
{
	// Détruire toutes les barres générées
	for (AActor* Bar : SpawnedBars)
	{
		if (Bar)
		{
			Bar->Destroy();
		}
	}

	SpawnedBars.Empty();
}

void AVisualizerActor::ResetAndRespawnVisualizer()
{
	ResetVisualizer();
	SpawnBars();
	
}

void AVisualizerActor::BeginCalculTest()
{
    UE_LOG(LogTemp, Warning, TEXT("BeginCalculTest called!"));

    if (!VisualizerSettings || 
        !VisualizerSettings->ConstantQNRT || 
        !VisualizerSettings->ConstantQNRT->Sound || 
        !VisualizerSettings->ConstantQNRT->Settings)
    {
        UE_LOG(LogTemp, Warning, TEXT("VisualizerSettings, ConstantQNRT, Sound, or Settings is null"));
        return;
    }
	//add name of the sound
	UE_LOG(LogTemp, Warning, TEXT("Sound name: %s"), *VisualizerSettings->ConstantQNRT->Sound->GetName());
    UConstantQNRT* QNRT = VisualizerSettings->ConstantQNRT;
    const float Duration = QNRT->Sound->Duration;
    const int32 NumBands = QNRT->Settings->NumBands;
	MusicDuration = QNRT->Sound->Duration;

    if (Duration <= 0 || NumBands <= 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("Invalid Sound duration or NumBands"));
        return;
    }

	//set the audio in the audio component
	AudioComponent->SetSound(CurrentSoundWave);
    // Tableau pour stocker les moyennes de chaque bande
    TArray<float> BandAverages;
    BandAverages.SetNum(NumBands);

    // Tableau temporaire pour stocker les valeurs de bande à chaque étape
    TArray<float> CurrentBandValues;
    const float AnalysisStep = 0.1f; // Intervalle en secondes pour l'analyse
    int32 TotalSteps = 0;

    for (float CurrentTime = 0.0f; CurrentTime < Duration; CurrentTime += AnalysisStep)
    {
        // Récupérer les valeurs de bande normalisées à l'instant `CurrentTime`
        QNRT->GetNormalizedChannelConstantQAtTime(CurrentTime, 0, CurrentBandValues);

        if (CurrentBandValues.Num() != NumBands)
        {
            UE_LOG(LogTemp, Warning, TEXT("Mismatch between NumBands and CurrentBandValues.Num()"));
            continue;
        }

        // Ajouter les valeurs actuelles aux moyennes
        for (int32 BandIndex = 0; BandIndex < NumBands; ++BandIndex)
        {
            BandAverages[BandIndex] += CurrentBandValues[BandIndex];
        }

        TotalSteps++;
    }

    // Diviser par le nombre total de pas pour obtenir la moyenne
    for (int32 BandIndex = 0; BandIndex < NumBands; ++BandIndex)
    {
        BandAverages[BandIndex] /= TotalSteps;
    }

    // Afficher les moyennes
    for (int32 BandIndex = 0; BandIndex < BandAverages.Num(); ++BandIndex)
    {
        UE_LOG(LogTemp, Log, TEXT("Band %d Average: %f"), BandIndex, BandAverages[BandIndex]);
    }

	//store
	CalculateThresholds(BandAverages);
}

void AVisualizerActor::CalculateThresholds(const TArray<float>& BandAverages)
{
	if (!VisualizerSettings || !VisualizerSettings->ConstantQNRT || !VisualizerSettings->ConstantQNRT->Settings)
	{
		UE_LOG(LogTemp, Warning, TEXT("VisualizerSettings, ConstantQNRT, or Settings is null"));
		return;
	}

	const int32 NumBands = VisualizerSettings->ConstantQNRT->Settings->NumBands;
	const float DifficultyMultiplier = 1.0f + (VisualizerSettings->Difficulty * 0.1f); // Exemple : x1.1 à x2.0

	VisualizerSettings->BandThresholds.Empty();
	for (int32 BandIndex = 0; BandIndex < NumBands; ++BandIndex)
	{
		float Threshold = BandAverages[BandIndex] * DifficultyMultiplier; // BandAverages est calculé dans BeginCalculTest
		VisualizerSettings->BandThresholds.Add(Threshold);
	}

	UE_LOG(LogTemp, Log, TEXT("Thresholds recalculated with difficulty %d"), VisualizerSettings->Difficulty);
	//afficher les seuils
	for (int32 BandIndex = 0; BandIndex < VisualizerSettings->BandThresholds.Num(); ++BandIndex)
	{
		UE_LOG(LogTemp, Log, TEXT("Band %d Threshold: %f"), BandIndex, VisualizerSettings->BandThresholds[BandIndex]);
	}
}

void AVisualizerActor::UpdateVisualizerAtTime( const float InSeconds)
{
	if (!PoolManager)
	{
		UE_LOG(LogTemp, Error, TEXT("PoolManager is NULL!"));
		return;
	}
	if (!VisualizerSettings || !VisualizerSettings->ConstantQNRT)
	{
		UE_LOG(LogTemp, Error, TEXT("VisualizerSettings or ConstantQNRT is NULL!"));
		return;
	}

	if (!VisualizerSettings->ConstantQNRT->Settings)
	{
		UE_LOG(LogTemp, Error, TEXT("ConstantQNRT Settings is NULL!"));
		return;
	}
	// Récupération des valeurs à l'instant donné
	TArray<float> BandValues;
	VisualizerSettings->ConstantQNRT->GetNormalizedChannelConstantQAtTime(InSeconds, 0, BandValues);

	// ✅ Mise à jour du visualiseur
	UpdateVisualizer(BandValues);

	// ✅ Vérification des seuils pour déclencher les SoundSpheres
	CheckThresholds(BandValues);
}


TArray<float> AVisualizerActor::CalculateBandAverages(const TArray<float>& BandValues, int32 NumSamplesPerBand)
{
	TArray<float> BandAverages;

	if (NumSamplesPerBand <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("NumSamplesPerBand must be greater than 0."));
		return BandAverages;
	}

	// Calculer le nombre total de bandes disponibles
	int32 TotalBands = BandValues.Num() / NumSamplesPerBand;

	if (TotalBands <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Not enough samples to calculate averages."));
		return BandAverages;
	}

	// Parcourir les bandes et calculer la moyenne pour chaque
	for (int32 BandIndex = 0; BandIndex < TotalBands; ++BandIndex)
	{
		float Sum = 0.0f;

		for (int32 SampleIndex = 0; SampleIndex < NumSamplesPerBand; ++SampleIndex)
		{
			int32 ValueIndex = BandIndex * NumSamplesPerBand + SampleIndex;
			if (ValueIndex < BandValues.Num())
			{
				Sum += BandValues[ValueIndex];
			}
		}

		float Average = Sum / NumSamplesPerBand;
		BandAverages.Add(Average);
	}

	return BandAverages;
}

void AVisualizerActor::CheckThresholds(const TArray<float>& BandValues)
{
	if (!VisualizerSettings || VisualizerSettings->BandThresholds.Num() != BandValues.Num() || !PoolManager)
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid VisualizerSettings, Thresholds, or PoolManager."));
		return;
	}

	for (int32 i = 0; i < BandValues.Num(); ++i)
	{
		// Mise à jour des cooldowns
		if (BandCooldownTimers[i] > 0.0f)
		{
			BandCooldownTimers[i] -= GetWorld()->GetDeltaSeconds();
		}

		// Vérifier si la valeur dépasse le seuil ET cooldown écoulé
		if (BandValues[i] > VisualizerSettings->BandThresholds[i] && BandCooldownTimers[i] <= 0.0f)
		{
			float SpeedMultiplier = BandValues[i] - VisualizerSettings->BandThresholds[i];

			// ✅ Spawner une SoundSphere en fonction du seuil franchi
			SpawnSoundSphereFromThreshold( i, SpeedMultiplier);

			// Réinitialiser le cooldown pour ce band
			BandCooldownTimers[i] = SpawnCooldown;
		}
	}
}

void AVisualizerActor::SpawnSoundSphereFromThreshold( int32 BandIndex, float SpeedMultiplier)
{
	if (!PoolManager)
	{
		UE_LOG(LogTemp, Error, TEXT("PoolManager is NULL! Cannot spawn SoundSphere."));
		return;
	}

	ASoundSphere* SoundSphere = Cast<ASoundSphere>(PoolManager->GetPooledObject());
	if (!SoundSphere)
	{
		UE_LOG(LogTemp, Warning, TEXT("No available SoundSphere in the pool."));
		return;
	}

	// Définir position et direction basées sur la barre du visualiseur
	if (SpawnedBars.IsValidIndex(BandIndex))
	{
		AActor* Bar = SpawnedBars[BandIndex];
		if (Bar)
		{
			FVector SpawnLocation = Bar->GetActorLocation();

			// Obtenir la rotation en Z de la barre
			FRotator BarRotation = Bar->GetActorRotation();
			float Yaw = BarRotation.Yaw; // Rotation sur Z (axe vertical)

			// Calculer la direction en utilisant la rotation en Z
			FVector Direction = FVector(FMath::Cos(FMath::DegreesToRadians(Yaw)), 
										FMath::Sin(FMath::DegreesToRadians(Yaw)), 
										0.0f); 

			// ✅ Configuration de l'objet
			SoundSphere->SetActorLocation(SpawnLocation);
			SoundSphere->SetDirection(Direction);
			SoundSphere->SetSpeed(1.0f + SpeedMultiplier);

			// ✅ Déterminer la couleur
			UMaterialInterface* SelectedMaterial = bIsColorWhite ? ColorMaterials.WhiteMaterial : ColorMaterials.BlackMaterial;
			SoundSphere->SetMaterialColor(SelectedMaterial);
			SoundSphere->SetColor(bIsColorWhite);
		}
	}
}


void AVisualizerActor::CheckLowFrequenciesAndChangeColor(const TArray<float>& BandValues)
{
	if (!VisualizerSettings || VisualizerSettings->BandThresholds.Num() != BandValues.Num())
	{
		UE_LOG(LogTemp, Warning, TEXT("Thresholds and BandValues count mismatch."));
		return;
	}

	// Plage des basses fréquences (bandes 0 à 3 par exemple)
	int32 LowFrequencyStart = 0;
	int32 LowFrequencyEnd = 3;

	// Vérifier les dépassements pour les basses fréquences
	for (int32 i = LowFrequencyStart; i <= LowFrequencyEnd; ++i)
	{
		if (BandValues[i] > VisualizerSettings->BandThresholds[i])
		{
			LowFrequencyExceedCount++;
			UE_LOG(LogTemp, Log, TEXT("Low frequency band %d exceeded threshold"), i);
		}
	}

	// Changer la couleur après avoir atteint le seuil
	if (LowFrequencyExceedCount >= ColorChangeThreshold)
	{
		bIsColorWhite = !bIsColorWhite; // Inverser l'état de la couleur
		LowFrequencyExceedCount = 0;   // Réinitialiser le compteur


		UMaterialInterface* NewMaterial = bIsColorWhite ? ColorMaterials.WhiteMaterial : ColorMaterials.BlackMaterial;

		for (AActor* Bar : SpawnedBars)
		{
			if (Bar)
			{
				UStaticMeshComponent* Mesh = Bar->FindComponentByClass<UStaticMeshComponent>();
				if (Mesh && NewMaterial)
				{
					Mesh->SetMaterial(0, NewMaterial);
				}
			}
		}

		UE_LOG(LogTemp, Log, TEXT("Sphere color changed to %s"), bIsColorWhite ? TEXT("White") : TEXT("Black"));
	}
}

float AVisualizerActor::CalculateGlobalAmplitude(const TArray<float>& BandValues)
{
	float Sum = 0.0f;

	for (float Value : BandValues)
	{
		Sum += Value;
	}

	// Retourner la moyenne ou la somme totale (vous pouvez ajuster selon votre logique)
	return Sum / BandValues.Num(); 
}

