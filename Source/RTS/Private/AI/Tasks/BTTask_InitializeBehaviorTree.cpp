// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Tasks/BTTask_InitializeBehaviorTree.h"
#include "AI/BasicAIController.h"

UBTTask_InitializeBehaviorTree::UBTTask_InitializeBehaviorTree()
{
	NodeName = TEXT("Initialize the Behavior Tree");
}

EBTNodeResult::Type UBTTask_InitializeBehaviorTree::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// Get Controller, Pawn and BlackBoard
	TObjectPtr<ABasicAIController> AIController{ Cast< ABasicAIController>(OwnerComp.GetAIOwner()) };
	if (!IsValid(AIController))
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return EBTNodeResult::Failed;
	}
	const TObjectPtr<APawn> AIPawn{ AIController->GetPawn() };
	if (!IsValid(AIPawn))
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return EBTNodeResult::Failed;
	}
	TObjectPtr<UBlackboardComponent> BlackboardComponent{ AIController->GetBlackboardComponent() };
	
	// TODO:: Initialize Code Here


	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	return EBTNodeResult::Succeeded;
}

FString UBTTask_InitializeBehaviorTree::GetStaticDescription() const
{
	return FString(TEXT("Initialize the behavior tree and blackboard"));
}
