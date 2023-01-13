// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/Formations/BasicFormation.h"
#include "ColumnFormation.generated.h"

/**
 * 
 */
UCLASS(BlueprintAble)
class RTS_API UColumnFormation : public UBasicFormation
{
	GENERATED_BODY()
	
public:
	UColumnFormation();
	TArray<FVector> GetLocationList(int num, float direction, FVector2D location, float unitBounds = 0) override;
	TArray<FVector> GetLocationList(int num, float direction, FVector2D location, TArray<float> unitBoundsList) override;
};
