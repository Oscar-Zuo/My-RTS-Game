// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Commands/BTTask_ClearCommand.h"
#include "AI/BasicAIController.h"
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

	TObjectPtr<UBlackboardComponent> blackboardComponent{ AIController->GetBlackboardComponent() };
	if (!IsValid(blackboardComponent))
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return EBTNodeResult::Failed;
	}

	// Try to clean the Command UObject
	TWeakObjectPtr<UObject> command = blackboardComponent->GetValueAsObject(FName(TEXT("NewCommand")));
	if (command.IsValid())
		command->MarkAsGarbage();
	blackboardComponent->ClearValue(FName(TEXT("NewCommand")));

	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	return EBTNodeResult::Succeeded;
}
