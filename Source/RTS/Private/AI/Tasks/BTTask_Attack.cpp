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

	auto selfPawn = OwnerComp->GetAIOwner()->GetPawn();
	IAttackableInterface::Execute_DisableWeapons(selfPawn);

	TWeakObjectPtr<UBlackboardComponent> blackBoradComponent = OwnerComp->GetBlackboardComponent();
	if (blackBoradComponent.IsValid())
	{
		TScriptInterface<IAttackableInterface> target = blackBoradComponent->GetValueAsObject(UBTTask_Attack::TARGET_BLACKBOARD_NAME);
		if (target)
			IAttackableInterface::Execute_RemoveFromAttackers(target.GetObject(), selfPawn);
	}

	UBTTask_BasicTask::StopTask(OwnerComp, Result);
}

EBTNodeResult::Type UBTTask_Attack::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	auto Controller = Cast<ABasicAIController>(OwnerComp.GetAIOwner());
	Controller->OnAttackTaskFinished.Unbind();

	if (auto selfPawn = OwnerComp.GetAIOwner()->GetPawn())
	{
		IAttackableInterface::Execute_DisableWeapons(selfPawn);

		TWeakObjectPtr<UBlackboardComponent> blackBoradComponent = OwnerComp.GetBlackboardComponent();
		if (blackBoradComponent.IsValid())
		{
			TScriptInterface<IAttackableInterface> target = blackBoradComponent->GetValueAsObject(UBTTask_Attack::TARGET_BLACKBOARD_NAME);
			if (target)
				IAttackableInterface::Execute_RemoveFromAttackers(target.GetObject(), selfPawn);
		}
	}

	return UBTTask_BasicTask::AbortTask(OwnerComp, NodeMemory);
}

EBTNodeResult::Type UBTTask_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	auto Controller = Cast<ABasicAIController>(OwnerComp.GetAIOwner());

	Controller->OnAttackTaskFinished.BindDynamic(this, &UBTTask_Attack::StopTask);

	auto selfPawn = OwnerComp.GetAIOwner()->GetPawn();
	IAttackableInterface::Execute_EnableWeapons(selfPawn);

	TWeakObjectPtr<UBlackboardComponent> blackBoradComponent = OwnerComp.GetBlackboardComponent();
	if (blackBoradComponent.IsValid())
	{
		TScriptInterface<IAttackableInterface> target = blackBoradComponent->GetValueAsObject(UBTTask_Attack::TARGET_BLACKBOARD_NAME);
		if (target)
			IAttackableInterface::Execute_SetAsAttacker(target.GetObject(), Cast<APawn>(selfPawn));
	}	

	return EBTNodeResult::InProgress;
}



FString UBTTask_Attack::GetStaticDescription() const
{
	return FString(TEXT(""));
}
