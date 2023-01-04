// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Commands/BasicCommand.h"

UBasicCommand::UBasicCommand()
{
}

bool UBasicCommand::NormalCommand(UBehaviorTreeComponent& OwnerComp)
{
	return false;
}

bool UBasicCommand::TargetActorCommand(UBehaviorTreeComponent& OwnerComp)
{
	return false;
}

bool UBasicCommand::TargetLocationCommand(UBehaviorTreeComponent& OwnerComp)
{
	return false;
}

FVector UBasicCommand::GetTargetLocation() const
{
	return targetLocation;
}

void UBasicCommand::SetTargetLocation(FVector _targetLocation)
{
	targetLocation = _targetLocation;
}

TObjectPtr<AActor> UBasicCommand::GetTargetActor() const
{
	return targetActor;
}

void UBasicCommand::SetTargetActor(TObjectPtr<AActor> _targetActor)
{
	targetActor = _targetActor;
}
