#include "ASoundSphere.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "AEnemyBoss.h"

ASoundSphere::ASoundSphere()
{
	PrimaryActorTick.bCanEverTick = true;

	// ✅ Création du composant de collision
	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));
	CollisionComponent->InitSphereRadius(10.0f);
	CollisionComponent->SetCollisionProfileName(TEXT("Projectile"));
	RootComponent = CollisionComponent;

	// ✅ Attacher l'événement de collision
	CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &ASoundSphere::OnOverlap);

	// ✅ Ajout du composant mesh
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(RootComponent);

}

void ASoundSphere::BeginPlay()
{
	Super::BeginPlay();
}

void ASoundSphere::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// ✅ Appliquer le déplacement seulement si actif
	if (Velocity != FVector::ZeroVector)
	{
		AddActorWorldOffset(Velocity * MoveSpeed * DeltaTime, true);

		// Vérifier la position X
		if (GetActorLocation().X > MaxXPosition)
		{
			StopMovement();
		}
	}
}

// ✅ Changer la couleur du projectile
void ASoundSphere::SetMaterialColor(UMaterialInterface* NewMaterial)
{
	if (NewMaterial && MeshComponent)
	{
		MeshComponent->SetMaterial(0, NewMaterial);
		UE_LOG(LogTemp, Log, TEXT("Material applied successfully!"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid material or MeshComponent is null!"));
	}
}


// ✅ Modifier la vitesse
void ASoundSphere::SetSpeed(float SpeedMultiplier)
{
	MoveSpeed *= SpeedMultiplier;
}

// ✅ Modifier la direction
void ASoundSphere::SetDirection(const FVector& Direction)
{
	Velocity = Direction.GetSafeNormal(); // Normaliser la direction pour éviter des bugs de scaling
}

// ✅ Tir du projectile (remplace `FireInDirection`)
void ASoundSphere::FireInDirection(const FVector& ShootDirection)
{
	SetDirection(ShootDirection);
}

void ASoundSphere::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
							  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, 
							  bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor != this)
	{
		// Check if the overlapping actor is the player
		APlayerController* PlayerController = Cast<APlayerController>(OtherActor->GetInstigatorController());
		if (PlayerController)
		{
			// Apply damage to the player
			UGameplayStatics::ApplyDamage(OtherActor, Damage, nullptr, this, UDamageType::StaticClass());
		}
	}
}
void ASoundSphere::Reset()
{
	bIsActive = true;
	Velocity = FVector::ZeroVector;
	MoveSpeed = 1200.0f;
    
	if (MeshComponent)
	{
		MeshComponent->SetVisibility(true);
	}
	if (CollisionComponent)
	{
		CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}

	SetActorLocation(FVector::ZeroVector);
}

void ASoundSphere::StopMovement()
{
	if (!bIsActive) return; // Don't stop if already stopped
    
	bIsActive = false;
	Velocity = FVector::ZeroVector;
	MoveSpeed = 0.0f;
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MeshComponent->SetVisibility(false);
	SetActorLocation(FVector::ZeroVector);
	Tags.Add(FName("Inactive"));
	Tags.Remove(FName("Active"));
}

void ASoundSphere::SetColor(bool bNewColor)
{
	bIsWhite = bNewColor;
}
