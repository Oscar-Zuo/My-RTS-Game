// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_BasicTask.generated.h"

#define COMMAND_FINISHED_NAME FName(TEXT("TASK_FINISHED"))

/**
 * 
 */
UCLASS()
class RTS_API UBTTask_BasicTask : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:
	UBTTask_BasicTask();
	void StopTask(UBehaviorTreeComponent& OwnerComp, EBTNodeResult::Type Result);
	void AbortTask(UBehaviorTreeComponent& OwnerComp);

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual FString GetStaticDescription() const override;
};
