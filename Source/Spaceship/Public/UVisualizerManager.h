#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h" // Utilisez DataAsset comme base
#include "AudioSynesthesia/Classes/ConstantQNRT.h"
#include "Sound/SoundWave.h"

#include "UVisualizerManager.generated.h"
UENUM(BlueprintType)
enum class EScaleAxis : uint8
{
	X UMETA(DisplayName = "X Axis"),
	Y UMETA(DisplayName = "Y Axis"),
	Z UMETA(DisplayName = "Z Axis")
};
UENUM(BlueprintType)
enum class EVisualizerShape : uint8
{
	Aligned UMETA(DisplayName = "Aligned"),
	Circle UMETA(DisplayName = "Circle"),
	HalfCircle UMETA(DisplayName = "Half Circle"),
	CustomCircle UMETA(DisplayName = "Custom Circle"),
	Max UMETA(DisplayName = "Max")
};

/**
 * Classe pour stocker les paramètres du visualiseur audio.
 */
UCLASS(Blueprintable)
class SPACESHIP_API UVisualizerManager : public UDataAsset
{
	GENERATED_BODY()

public:
	// Forme du visualiseur
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualizer")
    EVisualizerShape VisualizerShape = EVisualizerShape::Aligned;
	
	// Classe d'acteur représentant les barres du visualiseur
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualizer")
	TSubclassOf<AActor> BarActorClass;

	// Espacement entre les bandes
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualizer")
	float BandSpacing = 50.0f;

	// Instance de ConstantQNRT pour analyser le son
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualizer")
	UConstantQNRT* ConstantQNRT;

	// Seuil par bande (calculé dynamiquement, non exposé directement)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Visualizer", meta = (EditCondition = "false"))
	TArray<float> BandThresholds;

	// Difficulté du jeu (1-10)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualizer")
	int32 Difficulty = 5; // Un entier entre 1 et 10, par défaut à 5.
	
	// Rotation des barres
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualizer")
	FRotator BarRotation = FRotator(0.0f, 90.0f, 0.0f); // Par défaut, une rotation de 90° sur Y.

	// Axe d'alignement des barres (X, Y ou Z)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualizer")
	FVector AlignmentAxis = FVector(0.0f, 1.0f, 0.0f); // Par défaut, aligné sur l'axe Y.
	// Axe d'échelle des barres
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualizer")
	EScaleAxis ScaleAxis = EScaleAxis::Z; // Par défaut, échelle sur Z.

};


