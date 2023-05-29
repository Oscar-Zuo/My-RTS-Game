// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "AICommanderPawn.generated.h"

class UFaction;

UCLASS()
class RTS_API AAICommanderPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AAICommanderPawn();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Status)
	TObjectPtr<UFaction> Faction;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
