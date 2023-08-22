// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Tasks/BTTask_ClearCommand.h"
#include "AI/BasicAIController.h"
#include "AI/Tasks/BTTask_BasicTask.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_ClearCommand::UBTTask_ClearCommand()
{
	NodeName = TEXT("Clear the Command");
}

EBTNodeResult::Type UBTTask_ClearCommand::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	TObjectPtr<ABasicAIController> AIController{ Cast< ABasicAIController>(OwnerComp.GetAIOwner()) };
	if (!IsValid(AIController))
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return EBTNodeResult::Failed;
	}

	TObjectPtr<UBlackboardComponent> BlackboardComponent{ AIController->GetBlackboardComponent() };
	if (!IsValid(BlackboardComponent))
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return EBTNodeResult::Failed;
	}

	// Try to clean the Command UObject
	TWeakObjectPtr<UObject> command = BlackboardComponent->GetValueAsObject(UBTTask_BasicTask::COMMAND_BLACKBOARD_NAME);
	if (command.IsValid())
		command->MarkAsGarbage();
	BlackboardComponent->ClearValue(UBTTask_BasicTask::COMMAND_BLACKBOARD_NAME);

	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	return EBTNodeResult::Succeeded;
}
