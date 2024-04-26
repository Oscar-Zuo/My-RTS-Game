// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputState.h"
#include "AI/BasicAIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "BasicCharacter.generated.h"

/**
 * 
 */

class ABasicSquad;
class UNavModifierComponent;
class UFaction;

UCLASS()
class RTS_API ABasicCharacter : public ACharacter, public IAttackableInterface
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadWrite)
	bool IsAttacking;

protected:
	// Pointer to its squad
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<ABasicSquad> Squad;

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UMaterialInstanceDynamic> FogOfWarMaterialHiddenInstance;

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UMaterialInstanceDynamic> FogOfWarMaterialRevealInstance;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UMaterialInterface> FogOfWarHiddenMaterial;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UMaterialInterface> FogOfWarRevealMaterial;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UTextureRenderTarget2D> FogOfWarHiddenRender;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UTextureRenderTarget2D> FogOfWarRevealRender;

public:
	ABasicCharacter();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void ClearFogOfWar();

	UFUNCTION(BlueprintCallable)
	void UpdateRevealRadius();

	FORCEINLINE void SetSquad(const TWeakObjectPtr<ABasicSquad>& _squad);
	FORCEINLINE TWeakObjectPtr<ABasicSquad> GetSquad() const;

	// Helper functions
	FORCEINLINE TWeakObjectPtr<UFaction> GetFaction();

	// Helper functions
	UFUNCTION(BlueprintCallable)
	FORCEINLINE UFaction* GetFaction_BP();

	UFUNCTION(BlueprintCallable)
	virtual void OnReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser);

	// Attack interface implementation

	// Attack target if possible, returns true if target can be attacked, false if target can't be attacked
	virtual bool ConfirmAndAttackTarget_Implementation(const TScriptInterface<IAttackableInterface>& Target) override;
	virtual bool CanBeAttacked_Implementation(const TScriptInterface<IAttackableInterface>& Attacker, TSubclassOf<UDamageType> DamageType) override;
	virtual void AttackFeedBack_Implementation(EAttackResult Result, const TScriptInterface<IAttackableInterface>& Target) override;
	virtual void AttackTargetKilled_Implementation(const TScriptInterface<IAttackableInterface>& DyingTarget) override;
	FORCEINLINE virtual void EnableWeapons_Implementation() override;
	FORCEINLINE virtual void DisableWeapons_Implementation() override;
	FORCEINLINE void SetAsAttacker_Implementation(const TScriptInterface<IAttackableInterface>& Attacker) override;
	FORCEINLINE void RemoveFromAttackers_Implementation(const TScriptInterface<IAttackableInterface>& Attacker) override;
	virtual float TakeDamage_Implementation(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
