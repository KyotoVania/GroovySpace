#include "NiagaraProjectile.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h" // Required for spawning Niagara systems
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"
#include "AEnemyBoss.h" // Include if you need to check for boss hits specifically
#include "Engine/BlockingVolume.h"

ANiagaraProjectile::ANiagaraProjectile()
{
    PrimaryActorTick.bCanEverTick = false; // Usually projectiles don't need tick if movement component handles it

    // --- Create Components ---
    CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComp"));
    CollisionComp->InitSphereRadius(15.0f);
    CollisionComp->SetCollisionProfileName(TEXT("Projectile")); // Use a suitable profile
    CollisionComp->SetNotifyRigidBodyCollision(true); // Enable hit events
    RootComponent = CollisionComp;

    NiagaraComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraComp"));
    NiagaraComp->SetupAttachment(RootComponent);
    NiagaraComp->bAutoActivate = false; // Activate in BeginPlay after setting asset

    ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
    ProjectileMovement->UpdatedComponent = CollisionComp;
    ProjectileMovement->InitialSpeed = InitialSpeed;
    ProjectileMovement->MaxSpeed = MaxSpeed;
    ProjectileMovement->bRotationFollowsVelocity = true; // Mesh rotates to face movement direction
    ProjectileMovement->bShouldBounce = false; // Typically projectiles don't bounce
    ProjectileMovement->ProjectileGravityScale = 0.0f; // No gravity

    // Bind the OnHit function to the collision component's hit event
    CollisionComp->OnComponentHit.AddDynamic(this, &ANiagaraProjectile::OnHit);
}
void ANiagaraProjectile::BeginPlay()
{
    Super::BeginPlay();

    // Utiliser l'effet de la structure
    if (NiagaraEffects.ProjectileFX)
    {
        NiagaraComp->SetAsset(NiagaraEffects.ProjectileFX);
        NiagaraComp->Activate();
    }

    SetLifeSpan(Lifespan);

    // Assurer que les vitesses sont bien celles définies (au cas où elles changent après le constructeur)
    ProjectileMovement->InitialSpeed = InitialSpeed;
    ProjectileMovement->MaxSpeed = MaxSpeed;
}
void ANiagaraProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    // --- Vérifications initiales ---
    if (!OtherActor || OtherActor == this || !OtherComp)
    {
        // Ne rien faire si on se touche soi-même ou si l'autre acteur est invalide
        return;
    }

    // --- Vérification spéciale : Blocking Volume ---
    // Tente de caster l'acteur touché en ABlockingVolume (ou AVolume si plus général)
    AVolume* HitVolume = Cast<AVolume>(OtherActor); // Utilise AVolume pour plus de généralité
    if (HitVolume && HitVolume->IsA<ABlockingVolume>()) // Vérifie si c'est spécifiquement un BlockingVolume (ou un autre type si besoin)
    {
        // Si c'est un volume bloquant, on détruit juste le projectile sans effet d'impact
        Destroy();
        return; // Arrêter l'exécution ici
    }

    // --- Jouer les effets d'impact (si définis) ---
    bool bPlayedEffects = false;
    if (NiagaraEffects.ImpactFX)
    {
        // SpawnSystemAtLocation gère l'auto-destruction si le système Niagara est configuré pour
        UNiagaraFunctionLibrary::SpawnSystemAtLocation(
            GetWorld(),
            NiagaraEffects.ImpactFX,
            Hit.ImpactPoint,         // Position de l'impact
            Hit.ImpactNormal.Rotation() // Orienter l'effet selon la normale de l'impact
        );
        bPlayedEffects = true;
    }
    if (ImpactSound)
    {
        UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, Hit.ImpactPoint);
        bPlayedEffects = true;
    }
    
    // --- Vérification spécifique : AEnemyBoss ---
    AEnemyBoss* Boss = Cast<AEnemyBoss>(OtherActor);
    if (Boss)
    {
        // Appliquer les dégâts au boss
        UGameplayStatics::ApplyDamage(
            Boss,
            Damage,
            GetInstigatorController(), // Le controller qui a tiré (peut être récupéré via GetOwner()->GetInstigatorController())
            this, // L'acteur source des dégâts (le projectile)
            UDamageType::StaticClass() // Classe de dégâts par défaut
        );

        // TODO: Gérer l'ajout de score ici (probablement via un ScoreManager global)
        // Exemple : AScoreManager* ScoreManager = Cast<AScoreManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AScoreManager::StaticClass()));
        // if(ScoreManager) { ScoreManager->AddScore(PointsForBossHit); ScoreManager->IncrementCombo(); }
    }
    // --- Ajouter d'autres logiques de collision ici si nécessaire (autres ennemis, objets destructibles...) ---

    //check if we hit a player if so ignore the hit
    APlayerController* PlayerController = Cast<APlayerController>(OtherActor->GetInstigatorController());
    if (PlayerController)
    {
        // Si on touche un joueur, on ne fait rien
        return;
    }
    // --- Destruction du projectile ---
    // On détruit le projectile après avoir potentiellement joué les effets et appliqué les dégâts
    Destroy();
}

void ANiagaraProjectile::SetColorMode(bool bNewIsWhite)
{
    bIsWhite = bNewIsWhite;
    // You could potentially change Niagara parameters here based on color if needed
    // if(NiagaraComp) {
    //     NiagaraComp->SetNiagaraVariableLinearColor("User.Color", bIsWhite ? FLinearColor::White : FLinearColor::Black);
    // }
}

// Met à jour l'effet de projectile en vol
void ANiagaraProjectile::SetNiagaraAsset(UNiagaraSystem* NewFX)
{
    NiagaraEffects.ProjectileFX = NewFX; // Met à jour dans la structure
    if (NiagaraComp)
    {
        NiagaraComp->SetAsset(NewFX);
        if (IsActorInitialized()) // Vérifier si BeginPlay a déjà été appelé
        {
            NiagaraComp->Activate(true);
        }
    }
}