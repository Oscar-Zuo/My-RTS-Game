// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/Tasks/BTTask_TestBlackboardBase.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"
#include "AIController.h"
#include "AI/BasicAIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_TestBlackboardBase::UBTTask_TestBlackboardBase()
{
	NodeName = TEXT("Go to Destination");
}

EBTNodeResult::Type UBTTask_TestBlackboardBase::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	ABasicAIController* AIController{ Cast< ABasicAIController>(OwnerComp.GetAIOwner()) };
	if (!IsValid(AIController))
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return EBTNodeResult::Failed;
	}

	const APawn* AIPawn { AIController->GetPawn() };
	if (!IsValid(AIPawn))
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return EBTNodeResult::Failed;
	}

	// get our destination and nav system
	const FVector targetLocation{ AIController->GetBlackboardComponent()->GetValueAsVector(FName(TEXT("TargetLocation"))) };
	const UNavigationSystemV1* navSystem{ UNavigationSystemV1::GetCurrent(GetWorld()) };
	if (!IsValid(navSystem))
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return EBTNodeResult::Failed;
	}

	// judge if the location is reachable
	if (navSystem->FindPathToLocationSynchronously(GetWorld(), AIPawn->GetActorLocation(), targetLocation)->IsValid())
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return EBTNodeResult::Failed;
	}

	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	
	return EBTNodeResult::Succeeded;
}

FString UBTTask_TestBlackboardBase::GetStaticDescription() const
{
	return FString::Printf(TEXT("Vector: %s"), *BlackboardKey.SelectedKeyName.ToString());
}
