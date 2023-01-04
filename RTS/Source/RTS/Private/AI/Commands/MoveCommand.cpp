// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Commands/MoveCommand.h"
#include "Tasks/AITask_MoveTo.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UMoveCommand::UMoveCommand()
{
	acceptableRadius = 5.0f;
}

bool UMoveCommand::TargetLocationCommand(UBehaviorTreeComponent& OwnerComp)
{
	TObjectPtr<AAIController> controller = OwnerComp.GetAIOwner();
	EPathFollowingRequestResult::Type result = controller->MoveToLocation(targetLocation, acceptableRadius);
	if (result == EPathFollowingRequestResult::Failed)
		return false;
	else
		return true;
}
