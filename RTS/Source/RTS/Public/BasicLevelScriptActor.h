// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/LevelScriptActor.h"
#include "BasicLevelScriptActor.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class RTS_API ABasicLevelScriptActor : public ALevelScriptActor
{
	GENERATED_BODY()

public:
	ABasicLevelScriptActor();

	// the number of players in this level(including AI)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = GameInformation)
		int playersNum;

	// playerGroupsMap maps between player's id and player's group id
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = GameInformation)
		TMap<int, int> playersGroupsMap;
};
