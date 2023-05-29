// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Formations/ColumnFormation.h"

UColumnFormation::UColumnFormation()
{
	SpaceBetweenUnits = FVector2D(100, 100);
}

TArray<FVector> UColumnFormation::GetLocationList(int numOfSquadMembers, FVector2D faceDirection, FVector2D leaderLocation, float unitBounds)
{
	TArray<FVector> locationList;

	//We want them to line up
	FVector2D space = faceDirection * (SpaceBetweenUnits.X + unitBounds * 2);
	for (int i = 0; i < numOfSquadMembers; i++)
	{
		FVector2D planeLocation = leaderLocation - space * i;
		locationList.Add(GetRealLocation(planeLocation));
	}

	return locationList;
}

TArray<FVector> UColumnFormation::GetLocationList(int numOfSquadMembers, FVector2D faceDirection, FVector2D leaderLocation, TArray<float> unitBoundsList)
{
	TArray<FVector> locationList;

	//We want them to line up
	for (int i = 0; i < numOfSquadMembers; i++)
	{
		FVector2D space = faceDirection * (SpaceBetweenUnits.X + unitBoundsList[i] * 2);
		FVector2D planeLocation = leaderLocation + space * i;
		locationList.Add(GetRealLocation(planeLocation));
	}

	return locationList;
}
