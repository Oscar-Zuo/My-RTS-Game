// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Tasks/BTTask_Attack.h"
#include "AI/BasicAIController.h"

const FName UBTTask_Attack::TARGET_BLACKBOARD_NAME(TEXT("AttackTargetActor"));

UBTTask_Attack::UBTTask_Attack()
{
	NodeName = TEXT("Attack Target");
}

EBTNodeResult::Type UBTTask_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// Get Controller, Pawn and BlackBoard
	TWeakObjectPtr<ABasicAIController> AIController{ Cast< ABasicAIController>(OwnerComp.GetAIOwner()) };
	if (!AIController.IsValid())
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return EBTNodeResult::Failed;
	}

	// Need to let the controller know the pointer of this task, otherwise the controller can't stop it
	AIController->SetAttackTaskNode(this);
	AIController->EnableWeapon();

	return EBTNodeResult::InProgress;
}

FString UBTTask_Attack::GetStaticDescription() const
{
	return FString(TEXT(""));
}
