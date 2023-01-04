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

UCLASS(Blueprintable)
class RTS_API UBasicCommand : public UObject
{
	GENERATED_BODY()
	
public:
	UBasicCommand();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Status)
		TEnumAsByte<ECommandTypes> commandTypes;

	virtual bool NormalCommand(UBehaviorTreeComponent& OwnerComp);
	virtual bool TargetActorCommand(UBehaviorTreeComponent& OwnerComp);
	virtual bool TargetLocationCommand(UBehaviorTreeComponent& OwnerComp);

	FORCEINLINE FVector GetTargetLocation() const;
	FORCEINLINE void SetTargetLocation(FVector _targetLocation);
	FORCEINLINE TObjectPtr<AActor> GetTargetActor() const;
	FORCEINLINE void SetTargetActor(TObjectPtr<AActor> _targetActor);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Status)
		FVector targetLocation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Status)
		TObjectPtr<AActor> targetActor;
};
