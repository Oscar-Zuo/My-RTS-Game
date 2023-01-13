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
	// unitBound is collision radius of units
	virtual TArray<FVector> GetLocationList(int num, float direction, FVector2D location, float unitBounds = 0);
	virtual TArray<FVector> GetLocationList(int num, float direction, FVector2D location, TArray<float> unitBoundsList);
protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Status)
		FVector2D spaceBetweenUnits;
};
