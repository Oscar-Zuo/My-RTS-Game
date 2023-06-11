// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Tasks/BTTask_BasicTask.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AI/BasicAIController.h"
#include "AI/Commands/BasicCommand.h"

UBTTask_BasicTask::UBTTask_BasicTask()
{
	NodeName = TEXT("Perform Basic Task");

	// add filter for blackboard key
	BlackboardKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_BasicTask, BlackboardKey), ABasicAIController::StaticClass());
}

EBTNodeResult::Type UBTTask_BasicTask::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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

	TObjectPtr<UBasicCommand> commandObject{ Cast<UBasicCommand>(BlackboardComponent->GetValueAsObject(FName(TEXT("NewCommand")))) };
	if (!IsValid(commandObject))
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return EBTNodeResult::Failed;
	}

	// call performing funtion
	commandObject.Get()->Task = this;
	EBTNodeResult::Type bIsTaskSucceed = EBTNodeResult::Failed;
	switch (commandObject->commandTypes)
	{
	case NormalCommand:
		bIsTaskSucceed = commandObject->NormalCommand(OwnerComp);
		break;
	case TargetLocationCommand:
		bIsTaskSucceed = commandObject->TargetLocationCommand(OwnerComp);
		break;
	case TargetActorCommand:
		bIsTaskSucceed = commandObject->TargetActorCommand(OwnerComp);
		break;
	};

	if (bIsTaskSucceed != EBTNodeResult::InProgress)
	{
		FinishLatentTask(OwnerComp, bIsTaskSucceed);
	}
	return bIsTaskSucceed;
}

FString UBTTask_BasicTask::GetStaticDescription() const
{
	return FString::Printf(TEXT("Perform Task"));
}

void UBTTask_BasicTask::StopTask(UBehaviorTreeComponent& OwnerComp, EBTNodeResult::Type Result)
{
	FinishLatentTask(OwnerComp, Result);
}

void UBTTask_BasicTask::AbortTask(UBehaviorTreeComponent& OwnerComp)
{
	FinishLatentAbort(OwnerComp);
}
