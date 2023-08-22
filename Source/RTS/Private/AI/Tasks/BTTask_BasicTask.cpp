// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Tasks/BTTask_BasicTask.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AI/BasicAIController.h"
#include "AI/Commands/BasicCommand.h"

const FName UBTTask_BasicTask::COMMAND_BLACKBOARD_NAME(TEXT("NewCommand"));

UBTTask_BasicTask::UBTTask_BasicTask()
{
	NodeName = TEXT("Perform Basic Task");
	
	// add filter for blackboard key
	// BlackboardKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_BasicTask, BlackboardKey), ABasicAIController::StaticClass());
}

EBTNodeResult::Type UBTTask_BasicTask::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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

	TWeakObjectPtr<UBasicCommand> commandObject{ Cast<UBasicCommand>(BlackboardComponent->GetValueAsObject(COMMAND_BLACKBOARD_NAME)) };
	if (!commandObject.IsValid())
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
	return FinishLatentTask(OwnerComp, Result);
}

EBTNodeResult::Type UBTTask_BasicTask::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	FinishLatentAbort(OwnerComp);
	return Super::AbortTask(OwnerComp, NodeMemory);
}
