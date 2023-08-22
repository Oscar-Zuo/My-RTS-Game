// Fill out your copyright notice in the Description page of Project Settings.


#include "GameSettings/Faction.h"

UFaction::UFaction()
{
	DefaultRelationship = EFactionRelationship::kHostile;
	
}

TArray<TWeakObjectPtr<ACommanderPawn>> UFaction::GetPlayerList() const
{
	return PlayerList;
}

void UFaction::AddPlayer(const TWeakObjectPtr<ACommanderPawn>& player)
{
	PlayerList.Add(player);
}

void UFaction::AddPlayers(const TArray<TWeakObjectPtr<ACommanderPawn>>& newPlayerList)
{
	PlayerList.Append(newPlayerList);
}

bool UFaction::RemovePlayer(const TWeakObjectPtr<ACommanderPawn>& player)
{
	return PlayerList.RemoveSingle(player) > 0;
}

bool UFaction::ContainsPlayer(const TWeakObjectPtr<ACommanderPawn>& player)
{
	return PlayerList.Contains(player);
}

TArray<TWeakObjectPtr<AAICommanderPawn>> UFaction::GetAIList() const
{
	return AIList;
}

void UFaction::AddAI(const TWeakObjectPtr<AAICommanderPawn>& AI)
{
	AIList.Add(AI);
}

void UFaction::AddAIs(const TArray<TWeakObjectPtr<AAICommanderPawn>>& newAIList)
{
	AIList.Append(newAIList);
}

bool UFaction::RemoveAI(const TWeakObjectPtr<AAICommanderPawn>& AI)
{
	return AIList.RemoveSingle(AI) > 0;
}

bool UFaction::ContainsAI(const TWeakObjectPtr<AAICommanderPawn>& AI)
{
	return AIList.Contains(AI);
}

TEnumAsByte<EFactionRelationship> UFaction::GetRelationshipBetweenFactions(TWeakObjectPtr<UFaction> TargetFaction)
{
	TEnumAsByte<EFactionRelationship>* RelationshipPtr = RelationshipMap.Find(TargetFaction.Get());
	return RelationshipPtr ? *RelationshipPtr : DefaultRelationship;
}