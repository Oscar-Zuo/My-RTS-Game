// Fill out your copyright notice in the Description page of Project Settings.


#include "BasicLevelScriptActor.h"
#include "CommanderPawn.h"
#include "GameSettings/Faction.h"
#include "Kismet/GameplayStatics.h"

ABasicLevelScriptActor::ABasicLevelScriptActor()
{
}

TWeakObjectPtr<UFaction> ABasicLevelScriptActor::CreateNewFaction(const FName& name, EFactionRelationship DefaultRelationship)
{
	TObjectPtr<UFaction> NewFaction = NewObject<UFaction>(this, name);
	NewFaction->DefaultRelationship = DefaultRelationship;
	FactionList.Add(NewFaction);
	return NewFaction;
}

TWeakObjectPtr<UFaction> ABasicLevelScriptActor::FindFactionByName(const FName& name) const
{
	for (auto Faction : FactionList)
	{
		if (Faction && Faction->Name == name.ToString())
			return Faction;
	}
	return NULL;
}

TWeakObjectPtr<UBasicFormation> ABasicLevelScriptActor::AddFormation(const TSubclassOf<UBasicFormation>& NewFormation)
{
	return AllFormationsMap.FindOrAdd(NewFormation->GetFName(), NewObject<UBasicFormation>(this, NewFormation));
}

void ABasicLevelScriptActor::BeginPlay()
{
	// Create all formation objects
	for (auto formationClass : AllFormationClasses)
	{
		AllFormationsMap.FindOrAdd(formationClass->GetFName(), NewObject<UBasicFormation>(this, formationClass));
	}
}
