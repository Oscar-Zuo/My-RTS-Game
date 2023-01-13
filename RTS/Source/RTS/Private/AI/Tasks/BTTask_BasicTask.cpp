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
	TObjectPtr<UBlackboardComponent> blackboardComponent{ AIController->GetBlackboardComponent() };

	TObjectPtr<UBasicCommand> commandObject{ Cast<UBasicCommand>(blackboardComponent->GetValueAsObject(FName(TEXT("NewCommand")))) };
	if (!IsValid(commandObject))
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return EBTNodeResult::Failed;
	}

	// call performing funtion
	bool isTaskSucceed = false;
	switch (commandObject->commandTypes)
	{
	case NormalCommand:
		isTaskSucceed = commandObject->NormalCommand(OwnerComp);
		break;
	case TargetLocationCommand:
		isTaskSucceed = commandObject->TargetLocationCommand(OwnerComp);
		break;
	case TargetActorCommand:
		isTaskSucceed = commandObject->TargetActorCommand(OwnerComp);
		break;
	};

	if (!isTaskSucceed)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return EBTNodeResult::Failed;
	}
	
	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	return EBTNodeResult::Succeeded;
}

FString UBTTask_BasicTask::GetStaticDescription() const
{
	return FString::Printf(TEXT("Perform Task"));
}
