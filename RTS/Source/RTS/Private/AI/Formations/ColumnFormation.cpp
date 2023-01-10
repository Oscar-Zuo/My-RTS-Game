// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Formations/ColumnFormation.h"

UColumnFormation::UColumnFormation()
{
	spaceBetweenUnits = FVector2D(100, 100);
}

TArray<FVector2D> UColumnFormation::GetLocationList(int num, float direction, FVector2D location)
{
	TArray<FVector2D> locationList;

	//We want them to line up
	FVector2D space = FVector2D(spaceBetweenUnits.X, 0).GetRotated(direction);
	for (int i = 0; i < num; i++)
	{
		locationList.Add(location + space * i);
	}
	return locationList;
}
