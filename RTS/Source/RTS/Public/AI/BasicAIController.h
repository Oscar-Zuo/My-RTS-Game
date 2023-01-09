// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "AI/Formations/BasicFormation.h"
#include "BasicAIController.generated.h"

/**
 * The most basic AI Controller that includes moving ability
 */

class UMoveCommand;

class UBehaviorTreeComponent;

UCLASS(Blueprintable)
class RTS_API ABasicAIController : public AAIController
{
	GENERATED_BODY()

public:
	ABasicAIController();

	bool MoveToLocation(FVector Location);

protected:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AI", meta = (AllowPrivateAccess = true))
		TObjectPtr<UBehaviorTree> behaviorTree;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI", meta = (AllowPrivateAccess = true))
		TObjectPtr<UBehaviorTreeComponent> behaviorTreeComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI", meta = (AllowPrivateAccess = true))
		TObjectPtr<UBlackboardComponent> blackboardComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Status", meta = (AllowPrivateAccess = true))
		TSubclassOf<class UMoveCommand> CommandClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status")
		bool canBeSelect;
};