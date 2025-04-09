#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "FMaterialSet.h"
#include "SpaceshipSaveManager.h"
#include "Selector/SkinOptionsDataAsset.h"
#include "AObjectPoolManager.h"
#include "InputActionValue.h"
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

    // Input Action references
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    class UInputAction* MoveAction;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    class UInputAction* FireAction;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    class UInputAction* SwitchModeAction;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    class UInputAction* ExitShipAction;

    // Input Mapping Context
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    class UInputMappingContext* DefaultMappingContext;

    // Components
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    class UArrowComponent* ProjectileSpawnPointWhite;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    class UArrowComponent* ProjectileSpawnPointBlack;

    // Movement properties
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float ThrustForce = 1000.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float MaxRollAngle = 45.0f;

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

protected:
    virtual void BeginPlay() override;

    // Input handlers
    void Move(const FInputActionValue& Value);
    void HandleFire(const FInputActionValue& Value);
    void HandleSwitchMode(const FInputActionValue& Value);
    void HandleExitShip(const FInputActionValue& Value);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Customization")
    class USkinOptionsDataAsset* SkinOptions;

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

    void UpdateMaterials();
    void HandleSpaceshipMovement(float DeltaTime);
    void UpdateRotation(float RollInput, float PitchInput);
};