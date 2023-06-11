// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Commands/MoveCommand.h"
#include "Tasks/AITask_MoveTo.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UMoveCommand::UMoveCommand()
{
	acceptableRadius = 5.0f;
}

EBTNodeResult::Type UMoveCommand::TargetLocationCommand(UBehaviorTreeComponent& OwnerComp)
{
	TObjectPtr<AAIController> controller = OwnerComp.GetAIOwner();
	EPathFollowingRequestResult::Type result = controller->MoveToLocation(TargetLocation, acceptableRadius);
	if (result == EPathFollowingRequestResult::Failed)
		return EBTNodeResult::Failed;
	if (result == EPathFollowingRequestResult::AlreadyAtGoal)
		return EBTNodeResult::Succeeded;
	else
		return EBTNodeResult::InProgress;
}
