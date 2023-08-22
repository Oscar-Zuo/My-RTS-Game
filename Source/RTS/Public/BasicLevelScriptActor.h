// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/LevelScriptActor.h"
#include "Gamesettings/Faction.h"
#include "AI/Formations/BasicFormation.h"
#include "BasicLevelScriptActor.generated.h"

/**
 * 
 */

class ACommanderPawn;

UCLASS(Blueprintable)
class RTS_API ABasicLevelScriptActor : public ALevelScriptActor
{
	GENERATED_BODY()

public:
	ABasicLevelScriptActor();

	// playersFactionMap maps between player's id and player's faction id
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = GameInformations)
		TMap<TObjectPtr<ACommanderPawn>, TObjectPtr<UFaction>> PlayersFactionMap;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = GameInformations)
		TArray<TObjectPtr<UFaction>> FactionList;

	// formations to use in this game
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = GameInformations)
		TArray<TSubclassOf<UBasicFormation>> AllFormationClasses;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = GameInformations)
		TMap<FName, TObjectPtr<UBasicFormation>> AllFormationsMap;

public:
	TWeakObjectPtr<UFaction> CreateNewFaction(const FName& name, EFactionRelationship DefaultRelationship);
	// return NULL when can't find
	TWeakObjectPtr<UFaction> FindFactionByName(const FName& name) const;

	////return false if faction doesn't exit or invaild
	//bool SetAIToFaction(TObjectPtr<UFaction> Faction, TObjectPtr<AAICommanderPawn> AI);
	//bool SetPlayerToFaction(TObjectPtr<UFaction> Faction, TObjectPtr<ACommanderPawn> Player);

	TWeakObjectPtr<UBasicFormation> AddFormation(const TSubclassOf<UBasicFormation> &NewFormation);
protected:
	virtual void BeginPlay() override;
};
