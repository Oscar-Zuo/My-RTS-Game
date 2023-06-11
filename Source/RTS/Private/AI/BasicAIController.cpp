// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BasicAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AI/Commands/MoveCommand.h"
#include "Navigation/CrowdFollowingComponent.h"

ABasicAIController::ABasicAIController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UCrowdFollowingComponent>(TEXT("PathFollowingComponent")))
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

void ABasicAIController::OnMoveCompleted(FAIRequestID RequestID, EPathFollowingResult::Type Result)
{
	TObjectPtr<UMoveCommand> command = Cast<UMoveCommand>(BlackboardComponent->GetValueAsObject(FName(TEXT("NewCommand"))));
	if (!command)
		return;
	EBTNodeResult::Type TaskResult;
	switch (Result)
	{
	case EPathFollowingResult::Success:
		TaskResult = EBTNodeResult::Succeeded;
		break;
	case EPathFollowingResult::Blocked:
		TaskResult = EBTNodeResult::Failed;
		break;
	case EPathFollowingResult::OffPath:
		TaskResult = EBTNodeResult::Failed;
		break;
	case EPathFollowingResult::Aborted:
		command->AbortCommand();
		return;
	case EPathFollowingResult::Invalid:
		TaskResult = EBTNodeResult::Failed;
	default:
		return;
	}
	command->StopCommand(TaskResult);
}

bool ABasicAIController::SendMoveToLocationCommand(FVector Location)
{
	TObjectPtr<UMoveCommand> command = NewObject<UMoveCommand>(this, UMoveCommand::StaticClass());
	command->SetTargetLocation(Location);
	command->commandTypes = ECommandTypes::TargetLocationCommand;
	command->OwnerBehaviorTree = BehaviorTreeComponent;
	BlackboardComponent->SetValueAsObject(FName(TEXT("NewCommand")), command.Get());
	return true;
}
