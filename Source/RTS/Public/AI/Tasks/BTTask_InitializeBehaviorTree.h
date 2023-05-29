// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_InitializeBehaviorTree.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class RTS_API UBTTask_InitializeBehaviorTree : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:
	UBTTask_InitializeBehaviorTree();
protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual FString GetStaticDescription() const override;
};
