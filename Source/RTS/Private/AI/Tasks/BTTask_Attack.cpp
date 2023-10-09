// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Tasks/BTTask_Attack.h"
#include "AI/BasicAIController.h"

const FName UBTTask_Attack::TARGET_BLACKBOARD_NAME(TEXT("AttackTargetActor"));

UBTTask_Attack::UBTTask_Attack()
{
	NodeName = TEXT("Attack Target");
}

void UBTTask_Attack::StopTask(UBehaviorTreeComponent* OwnerComp, EBTNodeResult::Type Result)
{
	auto Controller = Cast<ABasicAIController>(OwnerComp->GetAIOwner());

	Controller->OnAttackTaskFinished.Unbind();

	IAttackableInterface::Execute_DisableWeapons(OwnerComp->GetAIOwner());

	TWeakObjectPtr<UBlackboardComponent> blackBoradComponent = OwnerComp->GetBlackboardComponent();
	if (blackBoradComponent.IsValid())
	{
		TWeakObjectPtr<AActor> target = Cast<AActor>(blackBoradComponent->GetValueAsObject(UBTTask_Attack::TARGET_BLACKBOARD_NAME));
		if (target.IsValid())
			IAttackableInterface::Execute_RemoveFromAttackers(target.Get(), OwnerComp->GetAIOwner());
	}

	UBTTask_BasicTask::StopTask(OwnerComp, Result);
}

EBTNodeResult::Type UBTTask_Attack::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	auto Controller = Cast<ABasicAIController>(OwnerComp.GetAIOwner());
	Controller->OnAttackTaskFinished.Unbind();

	IAttackableInterface::Execute_DisableWeapons(OwnerComp.GetAIOwner());

	TWeakObjectPtr<UBlackboardComponent> blackBoradComponent = OwnerComp.GetBlackboardComponent();
	if (blackBoradComponent.IsValid())
	{
		TWeakObjectPtr<AActor> target = Cast<AActor>(blackBoradComponent->GetValueAsObject(UBTTask_Attack::TARGET_BLACKBOARD_NAME));
		if (target.IsValid())
			IAttackableInterface::Execute_RemoveFromAttackers(target.Get(), OwnerComp.GetAIOwner());
	}

	return UBTTask_BasicTask::AbortTask(OwnerComp, NodeMemory);
}

EBTNodeResult::Type UBTTask_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	auto Controller = Cast<ABasicAIController>(OwnerComp.GetAIOwner());

	Controller->OnAttackTaskFinished.BindDynamic(this, &UBTTask_Attack::StopTask);
	IAttackableInterface::Execute_EnableWeapons(OwnerComp.GetAIOwner());

	TWeakObjectPtr<UBlackboardComponent> blackBoradComponent = OwnerComp.GetBlackboardComponent();
	if (blackBoradComponent.IsValid())
	{
		auto target = blackBoradComponent->GetValueAsObject(UBTTask_Attack::TARGET_BLACKBOARD_NAME);
		IAttackableInterface::Execute_SetAsAttacker(target, OwnerComp.GetAIOwner());
	}	

	return EBTNodeResult::InProgress;
}



FString UBTTask_Attack::GetStaticDescription() const
{
	return FString(TEXT(""));
}
