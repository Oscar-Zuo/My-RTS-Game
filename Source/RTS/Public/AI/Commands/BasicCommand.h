// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BasicCommand.generated.h"

/**
 * 
 */

UENUM()
enum ECommandTypes
{
	NormalCommand				UMETA(DisplayName = "Normal Command"),
	TargetActorCommand			UMETA(DisplayName = "Target is Actor Command"),
	TargetLocationCommand	UMETA(DisplayName = "Target is Location Command")
};

class UBTTask_BasicTask;

UCLASS(Blueprintable)
class RTS_API UBasicCommand : public UObject
{
	GENERATED_BODY()
	
public:
	TWeakObjectPtr<UBehaviorTreeComponent> OwnerBehaviorTree;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Status)
	TEnumAsByte<ECommandTypes> commandTypes;

	TWeakObjectPtr<UBTTask_BasicTask> Task;

public:
	UBasicCommand();

	virtual EBTNodeResult::Type NormalCommand(UBehaviorTreeComponent& OwnerComp);
	virtual EBTNodeResult::Type TargetActorCommand(UBehaviorTreeComponent& OwnerComp);
	virtual EBTNodeResult::Type TargetLocationCommand(UBehaviorTreeComponent& OwnerComp);

	FORCEINLINE virtual FVector GetTargetLocation() const;
	FORCEINLINE virtual void SetTargetLocation(FVector _targetLocation);
	FORCEINLINE virtual TObjectPtr<AActor> GetTargetActor() const;
	FORCEINLINE virtual void SetTargetActor(TObjectPtr<AActor> _targetActor);
	FORCEINLINE virtual void StopCommand(EBTNodeResult::Type result);
	FORCEINLINE virtual void AbortCommand();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Status)
		FVector TargetLocation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Status)
		TObjectPtr<AActor>TargetActor;
};
