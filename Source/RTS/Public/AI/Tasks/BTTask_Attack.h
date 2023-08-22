// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
//#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BTTask_BasicTask.h"
#include "BTTask_Attack.generated.h"

/**
 * Attack Target Task
 */
UCLASS()
class RTS_API UBTTask_Attack : public UBTTask_BasicTask
{
	GENERATED_BODY()
	
public:
	static const FName TARGET_BLACKBOARD_NAME;

public:
	UBTTask_Attack();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual FString GetStaticDescription() const override;
};
