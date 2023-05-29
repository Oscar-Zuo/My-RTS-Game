// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/AICommander/AICommanderPawn.h"

// Sets default values
AAICommanderPawn::AAICommanderPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Tags.Add(FName(TEXT("AICommander")));
}

// Called when the game starts or when spawned
void AAICommanderPawn::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AAICommanderPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
