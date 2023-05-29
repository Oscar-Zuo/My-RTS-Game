// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Formations/BasicFormation.h"

TArray<FVector> UBasicFormation::GetLocationList(int numOfSquadMembers, FVector2D faceDirection, FVector2D leaderLocation, float unitBounds)
{
	return TArray<FVector>();
}

TArray<FVector> UBasicFormation::GetLocationList(int numOfSquadMembers, FVector2D faceDirection, FVector2D leaderLocation, TArray<float> unitBoundsList)
{
	return TArray<FVector>();
}

FVector UBasicFormation::GetRealLocation(FVector2D planeLocation)
{
	// Need raycast to get the Z coordinate
	// which is not ideally, but I will try another way later.
	FVector targetLocation = FVector(planeLocation.X, planeLocation.Y, 0);
	FVector start = FVector(planeLocation.X, planeLocation.Y, 2000);
	FVector end = FVector(planeLocation.X, planeLocation.Y, -1000);
	FCollisionQueryParams collisionPara;
	collisionPara.MobilityType = EQueryMobilityType::Static;  // all we need is terrain
	FHitResult hit;
	if (GetWorld()->LineTraceSingleByChannel(hit, start, end, ECC_Visibility, collisionPara))
	{
		targetLocation = hit.Location;
	}
	return targetLocation;
}
