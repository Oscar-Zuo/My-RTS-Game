// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "AttackableInterface.generated.h"

UENUM(BlueprintType)
enum EAttackResult
{
	HIT		UMETA(DisplayName = "Hit"),
	MISS	UMETA(DisplayName = "Miss"),
	DEAD	UMETA(DisplayName = "Dead"),
	FAILED	UMETA(DisplayName = "Failed"),
};

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UAttackableInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class RTS_API IAttackableInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	// Attack Interface

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool CanAttack(const TScriptInterface<IAttackableInterface>& AttackerController);
	virtual bool CanAttack_Implementation(const TScriptInterface<IAttackableInterface>& AttackerController);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool ConfirmAndAttackTarget(const TScriptInterface<IAttackableInterface>& TargetController);
	virtual bool ConfirmAndAttackTarget_Implementation(const TScriptInterface<IAttackableInterface>& TargetController);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void AttackFeedBack(EAttackResult Result, const TScriptInterface<IAttackableInterface>& TargetController);
	virtual void AttackFeedBack_Implementation(EAttackResult Result, const TScriptInterface<IAttackableInterface>& TargetController);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void AttackTargetKilled(const TScriptInterface<IAttackableInterface>& DyingTargetController);
	virtual void AttackTargetKilled_Implementation(const TScriptInterface<IAttackableInterface>& DyingTargetController);

	// Used in Attack Task Node, will not check requirements
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void EnableWeapons();
	virtual void EnableWeapons_Implementation();

	// Used in Attack Task Node
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void DisableWeapons();
	virtual void DisableWeapons_Implementation();

	// Reveive Attack Interface

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool CanBeAttacked(const TScriptInterface<IAttackableInterface>& AttackerController, TSubclassOf<UDamageType> DamageType);
	// Returns if the actor can be attack
	virtual bool CanBeAttacked_Implementation(const TScriptInterface<IAttackableInterface>& AttackerController, TSubclassOf<UDamageType> DamageType);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void SetAsAttacker(const TScriptInterface<IAttackableInterface>& AttackerController);
	virtual void SetAsAttacker_Implementation(const TScriptInterface<IAttackableInterface>& AttackerController);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void RemoveFromAttackers(const TScriptInterface<IAttackableInterface>& AttackerController);
	virtual void RemoveFromAttackers_Implementation(const TScriptInterface<IAttackableInterface>& AttackerController);
};
