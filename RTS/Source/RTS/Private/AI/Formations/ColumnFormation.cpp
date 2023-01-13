// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Formations/ColumnFormation.h"

UColumnFormation::UColumnFormation()
{
	spaceBetweenUnits = FVector2D(100, 100);
}

TArray<FVector> UColumnFormation::GetLocationList(int num, float direction, FVector2D location, float unitBounds)
{
	TArray<FVector> locationList;

	//We want them to line up
	FVector2D space = FVector2D(spaceBetweenUnits.X + 2 * unitBounds, 0).GetRotated(direction);
	for (int i = 0; i < num; i++)
	{
		FVector2D location2D = location + space * i;

		// Need raycast to get the Z coordinate
		// which is not ideally, but I will try another way later.
		FVector targetLocation = FVector(location2D.X, location2D.Y, 0);
		FVector start = FVector(location2D.X, location2D.Y, 2000);
		FVector end = FVector(location2D.X, location2D.Y, -1000);
		FCollisionQueryParams collisionPara;
		collisionPara.MobilityType = EQueryMobilityType::Static;  // all we need is terrain
		FHitResult hit;
		if (GetWorld()->LineTraceSingleByChannel(hit, start, end, ECC_Visibility, collisionPara))
		{
			targetLocation = hit.Location;
		}
		locationList.Add(targetLocation);
	}

	return locationList;
}

TArray<FVector> UColumnFormation::GetLocationList(int num, float direction, FVector2D location, TArray<float> unitBoundsList)
{
	TArray<FVector> locationList;

	//We want them to line up
	for (int i = 0; i < num; i++)
	{
		FVector2D space = FVector2D(spaceBetweenUnits.X + 2 * unitBoundsList[i], 0).GetRotated(direction);
		FVector2D location2D = location + space * i;

		// Need raycast to get the Z coordinate
		// which is not ideally, but I will try another way later.
		FVector targetLocation = FVector(location2D.X, location2D.Y, 0);
		FVector start = FVector(location2D.X, location2D.Y, 2000);
		FVector end = FVector(location2D.X, location2D.Y, -1000);
		FCollisionQueryParams collisionPara;
		collisionPara.MobilityType = EQueryMobilityType::Static;  // all we need is terrain
		FHitResult hit;
		if (GetWorld()->LineTraceSingleByChannel(hit, start, end, ECC_Visibility, collisionPara))
		{
			targetLocation = hit.Location;
		}
		locationList.Add(targetLocation);
	}

	return locationList;
}
