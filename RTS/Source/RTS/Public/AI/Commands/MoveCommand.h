// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/Commands/BasicCommand.h"
#include "MoveCommand.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class RTS_API UMoveCommand : public UBasicCommand
{
	GENERATED_BODY()
	
public:
	UMoveCommand();
	bool TargetLocationCommand(UBehaviorTreeComponent& OwnerComp) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Status)
	float acceptableRadius;

};
