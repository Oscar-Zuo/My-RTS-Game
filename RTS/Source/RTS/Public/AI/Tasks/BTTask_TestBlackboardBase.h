// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_TestBlackboardBase.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class RTS_API UBTTask_TestBlackboardBase : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:
	UBTTask_TestBlackboardBase();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual FString GetStaticDescription() const override;

	//UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Search", meta = (AllowPrivateAccess = true))
	//float Search
};
