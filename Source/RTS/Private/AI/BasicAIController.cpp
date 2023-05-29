// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BasicAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AI/Commands/MoveCommand.h"

ABasicAIController::ABasicAIController()
{
	BehaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("Behavior Tree Component"));
	BlackboardComponent = CreateDefaultSubobject<UBlackboardComponent>(TEXT("Blackboard Component"));
	canBeSelect = true;
}

void ABasicAIController::BeginPlay()
{
	Super::BeginPlay();
	// initialize the behavior tree
	if (IsValid(BehaviorTree.Get()))
	{
		RunBehaviorTree(BehaviorTree.Get());
		BehaviorTreeComponent->StartTree(*BehaviorTree.Get());
	}
}

void ABasicAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (IsValid(Blackboard.Get()) && IsValid(BehaviorTree))
	{
		Blackboard->InitializeBlackboard(*BehaviorTree.Get()->BlackboardAsset.Get());
	}
}

bool ABasicAIController::MoveToLocation(FVector Location)
{
	TWeakObjectPtr<UMoveCommand> command = NewObject<UMoveCommand>(this, UMoveCommand::StaticClass());
	command->SetTargetLocation(Location);
	command->commandTypes = ECommandTypes::TargetLocationCommand;
	BlackboardComponent->SetValueAsObject(FName(TEXT("NewCommand")), command.Get());

	return true;
}
