// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "BasicFormation.generated.h"

/**
 * 
 */

UCLASS(Blueprintable)
class RTS_API UBasicFormation : public UObject
{
	GENERATED_BODY()
	
public:
	virtual TArray<FVector2D> GetLocationList(int num, float direction, FVector2D location);

protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Status)
		FVector2D spaceBetweenUnits;
};
