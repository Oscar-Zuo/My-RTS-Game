// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/Commands/BasicCommand.h"
#include "AI/Tasks/BTTask_BasicTask.h"

UBasicCommand::UBasicCommand()
{
}

EBTNodeResult::Type UBasicCommand::NormalCommand(UBehaviorTreeComponent& OwnerComp)
{
	return EBTNodeResult::Failed;
}

EBTNodeResult::Type UBasicCommand::TargetActorCommand(UBehaviorTreeComponent& OwnerComp)
{
	return EBTNodeResult::Failed;
}

EBTNodeResult::Type UBasicCommand::TargetLocationCommand(UBehaviorTreeComponent& OwnerComp)
{
	return EBTNodeResult::Failed;
}

FVector UBasicCommand::GetTargetLocation() const
{
	return TargetLocation;
}

void UBasicCommand::SetTargetLocation(FVector _targetLocation)
{
	TargetLocation = _targetLocation;
}

TObjectPtr<AActor> UBasicCommand::GetTargetActor() const
{
	return TargetActor;
}

void UBasicCommand::SetTargetActor(TObjectPtr<AActor> _targetActor)
{
	TargetActor = _targetActor;
}

void UBasicCommand::StopCommand(EBTNodeResult::Type result)
{
	if (Task.IsValid() && OwnerBehaviorTree.IsValid())
		Task.Get()->StopTask(*OwnerBehaviorTree, result);
}

void UBasicCommand::AbortCommand()
{
	if (Task.IsValid() && OwnerBehaviorTree.IsValid())
		Task.Get()->AbortTask(*OwnerBehaviorTree, NULL);
}
