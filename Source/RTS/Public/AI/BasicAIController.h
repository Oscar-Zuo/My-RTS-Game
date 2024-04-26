// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Pawn/AttackableInterface.h"
#include "Tasks/BTTask_Attack.h"
#include "BasicAIController.generated.h"

DECLARE_DYNAMIC_DELEGATE_TwoParams(FinishAttackTask, UBehaviorTreeComponent*, OwnerComp, EBTNodeResult::Type, Result);

/**
 * The most basic AI Controller that includes moving ability
 */

class UMoveCommand;

class UBehaviorTreeComponent;

UCLASS(Blueprintable)
class RTS_API ABasicAIController : public AAIController
{
	GENERATED_BODY()

public:
	FinishAttackTask OnAttackTaskFinished;
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
	float VisionRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status")
	float Health;

	UPROPERTY(BlueprintReadWrite, Category = "Status")
	TArray<TScriptInterface<IAttackableInterface>> AttackersController;

	// TODO::Should use weapon slot
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	bool CanAttack;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	float AttackRange;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	float WeaponDamage;
	// pre ms
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	float AttackInterval;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	TSubclassOf<UDamageType> DamageTypeClass;

private:
	FTimerHandle WeaponCountdownTimerHandle;

	UPROPERTY(Category = Character, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAIPerceptionComponent> AIPresecptionComponent;

public:
	ABasicAIController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	bool SendMoveToLocationCommand(FVector Location);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetVisionRadius() const;

	UFUNCTION(BlueprintCallable)
	void SetVisionRadius(float newRadius);

	FORCEINLINE void StopAndClearAllCommand();

	UFUNCTION(BlueprintCallable)
	void ClearAttackTarget();

	UFUNCTION(BlueprintCallable)
	virtual void OnReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser);

	UFUNCTION(BlueprintCallable)
	TScriptInterface<IAttackableInterface> GetAttackingTarget() const;

	// Attack interface implementation

	// Attack target if possible, returns true if target can be attacked, false if target can't be attacked
	virtual bool ConfirmAndAttackTarget(const TScriptInterface<IAttackableInterface>& Target);
	virtual bool CanBeAttacked(const TScriptInterface<IAttackableInterface>& Attacker, TSubclassOf<UDamageType> DamageType);
	virtual void AttackFeedBack(EAttackResult Result, const TScriptInterface<IAttackableInterface>& Target);
	virtual void AttackTargetKilled(const TScriptInterface<IAttackableInterface>& DyingTarget);
	FORCEINLINE virtual void EnableWeapons();
	FORCEINLINE virtual void DisableWeapons();
	FORCEINLINE void SetAsAttacker(const TScriptInterface<IAttackableInterface>& Attacker);
	FORCEINLINE void RemoveFromAttackers(const TScriptInterface<IAttackableInterface>& Attacker);

protected:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnMoveCompleted(FAIRequestID RequestID, EPathFollowingResult::Type Result) override;
	virtual void Dead(TWeakObjectPtr<AActor> Causer);

private:
	// Set Attack Target to be attacked
	void SetAttackTarget(const TScriptInterface<IAttackableInterface>& Target);

	// Force Attack Target, USE ConfirmAndAttackTarget AT MOST CIRCUMSTANCES
	virtual void PerformAttackTarget();

};
