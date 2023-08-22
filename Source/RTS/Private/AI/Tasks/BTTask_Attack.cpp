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

	const TWeakObjectPtr<APawn> AIPawn{ AIController->GetPawn() };
	if (!AIPawn.IsValid())
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return EBTNodeResult::Failed;
	}
	
	TWeakObjectPtr<UBlackboardComponent> BlackboardComponent{ AIController->GetBlackboardComponent() };
	if (!BlackboardComponent.IsValid())
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return EBTNodeResult::Failed;
	}

	AIController->EnableWeapon();

	return EBTNodeResult::InProgress;
}

FString UBTTask_Attack::GetStaticDescription() const
{
	return FString(TEXT(""));
}
