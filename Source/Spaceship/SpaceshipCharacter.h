#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
// Required Includes for new features
#include "GameFramework/SpringArmComponent.h"
#include "FMaterialSet.h"
#include "SpaceshipSaveManager.h"
#include "Selector/SkinOptionsDataAsset.h"
// --- AJOUT ---
#include "FVFXOption.h" // <-- Inclure pour UVFXOptionsDataAsset et FNiagaraEffectPair
// --- FIN AJOUT ---
#include "AObjectPoolManager.h"
#include "InputActionValue.h"
#include "NiagaraProjectile.h"
#include "NiagaraComponent.h"
#include "Camera/CameraComponent.h" // Pour UCameraComponent
#include "GameFramework/SpringArmComponent.h" // Pour USpringArmComponent (déjà inclus mais bon à vérifier)
#include "InputMappingContext.h" // Pour UInputMappingContext
#include "InputAction.h" // Pour UInputAction*
#include "Components/ArrowComponent.h" // Pour UArrowComponent*
#include "Camera/CameraShakeBase.h" // Pour TSubclassOf<UCameraShakeBase>
#include "Particles/ParticleSystem.h" // Pour UParticleSystem* (même si c'est un effet Legacy)
#include "Sound/SoundBase.h" // Pour USoundBase*
#include "Materials/MaterialInterface.h" // Pour UMaterialInterface*
#include "Materials/MaterialInstanceDynamic.h" // Pour UMaterialInstanceDynamic*
#include "SpaceshipCharacter.generated.h"

UCLASS()
class SPACESHIP_API ASpaceshipCharacter : public ACharacter
{
	GENERATED_BODY()

public:
    ASpaceshipCharacter();

    virtual void Tick(float DeltaTime) override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
    virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

    // --- Camera Components ---
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
    USpringArmComponent* SpringArmComp;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
    UCameraComponent* CameraComp;

    // --- Camera Settings ---
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Camera")
    float CameraBoomLength = 300.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Camera")
    bool bEnableCameraLag = true;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Camera", meta = (EditCondition = "bEnableCameraLag"))
    float CameraLagSpeed = 10.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Camera", meta = (EditCondition = "bEnableCameraLag"))
    bool bEnableCameraRotationLag = true;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Camera", meta = (EditCondition = "bEnableCameraRotationLag"))
    float CameraRotationLagSpeed = 10.0f;


    // --- Hit Effect System ---

    // Class of camera shake to play when hit
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Hit Effects")
    TSubclassOf<UCameraShakeBase> HitCameraShakeClass;

    // Post-process material to apply briefly on hit
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Hit Effects")
    UMaterialInterface* HitPostProcessMaterial;

    // Particle effect to spawn at ship location on hit (optional)
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Hit Effects")
    UParticleSystem* HitParticleEffect;

    // Sound effect to play on hit
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Hit Effects")
    USoundBase* HitSound;

    // Parameter name in HitPostProcessMaterial to control intensity (if any)
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Hit Effects")
    FName HitEffectIntensityParamName = TEXT("Intensity");

    // Duration the hit post-process effect stays active
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Hit Effects")
    float HitEffectDuration = 0.2f;


    // --- Existing Components & Properties ---

    // Input Action references
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    UInputAction* MoveAction;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    UInputAction* FireAction;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    UInputAction* SwitchModeAction;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    UInputAction* ExitShipAction;

    // Input Mapping Context
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    UInputMappingContext* DefaultMappingContext;

    // Components
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UArrowComponent* ProjectileSpawnPointWhite;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UArrowComponent* ProjectileSpawnPointBlack;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Projectile")
    TSubclassOf<ANiagaraProjectile> ProjectileClass; // Une seule classe pour tous les tirs

    // --- AJOUT : Référence au Data Asset des options VFX ---
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "VFX")
    UVFXOptionsDataAsset* VFXOptions;
    // --- FIN AJOUT ---


    // Movement properties
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float ThrustForce = 1000.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float MaxRollAngle = 45.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float MaxPitchAngle = 25.0f;
    // Gameplay properties
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
    bool bIsWhiteMode = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
    bool bIsInSpaceship = true;

    // Functions
    UFUNCTION(BlueprintCallable, Category = "Gameplay")
    void FireProjectile();

    UFUNCTION(BlueprintCallable, Category = "Gameplay")
    void ToggleColor();

    UFUNCTION(BlueprintCallable, Category = "Gameplay")
    void ToggleSpaceshipMode();

    UFUNCTION(BlueprintCallable, Category = "Gameplay")
    void Enter(ACharacter* Character);

    UFUNCTION(BlueprintCallable, Category = "Gameplay")
    void Exit();

    // --- AJOUT : Fonction pour récupérer la paire de VFX sélectionnée ---
    UFUNCTION(BlueprintPure, Category = "VFX") // BlueprintPure car ne modifie rien
    FNiagaraEffectPair GetSelectedVFXPair() const;
    // --- FIN AJOUT ---

    //Thruster
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VFX")
    class UNiagaraComponent* LeftThrusterVFX;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VFX")
    class UNiagaraComponent* RightThrusterVFX;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VFX")
    class UNiagaraSystem* ThrusterEffect;

protected:
    virtual void BeginPlay() override;
    bool bCanToggleColor;
	UPROPERTY(EditDefaultsOnly, Category = "Health")
	float MaxHealth = 100.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Health")
	float DamagePerHit = 33.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Health|Regeneration")
	float HealthRegenRate = 5.0f;  // HP par seconde

	UPROPERTY(EditDefaultsOnly, Category = "Health|Regeneration")
	float HealthRegenDelay = 3.0f; // Délai avant régénération en secondes

	UPROPERTY()
	float CurrentHealth;

	FTimerHandle HealthRegenTimerHandle;
	FTimerHandle HealthRegenDelayHandle;
	void StartHealthRegeneration();
	void StopHealthRegeneration();
	void RegenerateHealth();
    // Handle pour le timer du cooldown
    FTimerHandle ColorToggleCooldownTimerHandle;
    FVector2D CurrentMovementInput; // Stocker l'input actuel

    // Durée du cooldown (exposable en Blueprint pour ajustement facile)
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Gameplay|Cooldowns")
    float ColorToggleCooldownDuration = 0.3f;

    // Fonction appelée à la fin du cooldown pour réactiver le changement
    void ResetColorToggleCooldown();
    // Input handlers
    void Move(const FInputActionValue& Value);
    void HandleFire(const FInputActionValue& Value);
    void HandleSwitchMode(const FInputActionValue& Value);
    void HandleExitShip(const FInputActionValue& Value);

    // Hit effect handler
    void HandleHit(float DamageAmount, FDamageEvent const& DamageEvent, AActor* DamageCauser);

    // Function to apply and then remove the hit post-process effect
    void TriggerHitPostProcess(float Intensity);
    void ClearHitPostProcess();
    FTimerHandle HitEffectTimerHandle;


    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Customization")
    USkinOptionsDataAsset* SkinOptions;

    UPROPERTY()
    USpaceshipSaveManager* SaveManager;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
    ACharacter* PlayerCharacter;

    UPROPERTY()
    class AObjectPoolManager* PoolManager;

    UPROPERTY(EditDefaultsOnly, Category = "Combat")
    float FireCooldown = 0.2f;

    FTimerHandle FireCooldownTimer;
    bool bCanFire = true;

    FMaterialSet CurrentMaterials;

    // Helper Functions
    void UpdateMaterials();
    void HandleSpaceshipMovement(float DeltaTime);
    void UpdateRotation(float RollInput, float PitchInput);
    // Store dynamic material instance for hit effect
    UPROPERTY()
    class UMaterialInstanceDynamic* HitMID;
    static constexpr float THRUSTER_FORCE_MULTIPLIER = 2.0f;
    static constexpr float PARTICULATE_FORCE_MULTIPLIER = 1.0f;
    static constexpr float ENERGY_CORE_FORCE_MULTIPLIER = 0.25f; // Division par 4
    static constexpr float HEAT_HAZE_FORCE_MULTIPLIER = 1.5f;
     FName ThrustersSpeedParam;
      FName ParticulateSpeedParam ;
      FName EnergyCoreSpeedParam;
      FName HeatHazeSpeedParam ;
    UPROPERTY(BlueprintReadOnly, Category = "VFX")
    float ThrustersSpeed;

    UPROPERTY(BlueprintReadOnly, Category = "VFX")
    float ParticulateSpeed;

    UPROPERTY(BlueprintReadOnly, Category = "VFX")
    float EnergyCoreSpeed;

    UPROPERTY(BlueprintReadOnly, Category = "VFX")
    float HeatHazeSpeed;

    void UpdateThrusterParametersVisuals(float DeltaTime); // Prend DeltaTime si besoin pour interpolation

};