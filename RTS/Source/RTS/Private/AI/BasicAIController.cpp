// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BasicAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AI/Commands/MoveCommand.h"

ABasicAIController::ABasicAIController()
{
	behaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("Behavior Tree Component"));
	blackboardComponent = CreateDefaultSubobject<UBlackboardComponent>(TEXT("Blackboard Component"));
	canBeSelect = true;
}

void ABasicAIController::BeginPlay()
{
	Super::BeginPlay();
	// initialize the behavior tree
	if (IsValid(behaviorTree.Get()))
	{
		RunBehaviorTree(behaviorTree.Get());
		behaviorTreeComponent->StartTree(*behaviorTree.Get());
	}
}

void ABasicAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (IsValid(Blackboard.Get()) && IsValid(behaviorTree))
	{
		Blackboard->InitializeBlackboard(*behaviorTree.Get()->BlackboardAsset.Get());
	}
}

bool ABasicAIController::MoveToLocation(FVector Location)
{
	TWeakObjectPtr<UMoveCommand> command = NewObject<UMoveCommand>(this, UMoveCommand::StaticClass());
	command->SetTargetLocation(Location);
	command->commandTypes = ECommandTypes::TargetLocationCommand;
	blackboardComponent->SetValueAsObject(FName(TEXT("NewCommand")), command.Get());

	return true;
}
