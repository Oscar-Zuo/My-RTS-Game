// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Pawn/AttackableInterface.h"
#include "Tasks/BTTask_BasicTask.h"
#include "BasicAIController.generated.h"

/**
 * The most basic AI Controller that includes moving ability
 */

class UMoveCommand;

class UBehaviorTreeComponent;

UCLASS(Blueprintable)
class RTS_API ABasicAIController : public AAIController
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AI", meta = (AllowPrivateAccess = true))
		TObjectPtr<UBehaviorTree> BehaviorTree;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI", meta = (AllowPrivateAccess = true))
		TObjectPtr<UBehaviorTreeComponent> BehaviorTreeComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI", meta = (AllowPrivateAccess = true))
		TObjectPtr<UBlackboardComponent> BlackboardComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status")
		bool bCanBeSelect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status")
		float Health;

	// TODO::Should use weapon slot
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		bool CanAttack;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		float AttackRange;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		float AttackDamage;
	// pre ms
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		float AttackInterval;

private:
	FTimerHandle WeaponCountdownTimerHandle;

public:
	ABasicAIController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	bool SendMoveToLocationCommand(FVector Location);

	FORCEINLINE void StopAndClearAllCommand();

	// Attack target if possible, returns true if target can be attacked, false if target can't be attacked
	UFUNCTION(BlueprintCallable)
		bool ConfirmAndAttackTarget(const TScriptInterface<IAttackableInterface>& Target);

	UFUNCTION(BlueprintCallable)
		void ClearAttackTarget();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool CanBeAttacked(AActor* Attacker);
	virtual bool CanBeAttacked_Implementation(TWeakObjectPtr<AActor> Attacker);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
		void ReceiveDamage(AActor* Attacker, float Damage);
	virtual TEnumAsByte<EAttackResult> ReceiveDamage_Implementation(TWeakObjectPtr<AActor> Attacker, float Damage);

	FORCEINLINE virtual void EnableWeapon();
	FORCEINLINE virtual void DisableWeapon();

protected:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnMoveCompleted(FAIRequestID RequestID, EPathFollowingResult::Type Result) override;

private:
	// Set Attack Target to be attacked
	void SetAttackTarget(const TScriptInterface<IAttackableInterface>& Target);

	// Force Attack Target, USE ConfirmAndAttackTarget AT MOST CIRCUMSTANCES
	virtual void PerformAttackTarget();
};
