// Fill out your copyright notice in the Description page of Project Settings.


#include "GameSettings/Faction.h"

UFaction::UFaction()
{
	DefaultRelationship = EFactionRelationship::kHostile;
	
}

TArray<TObjectPtr<ACommanderPawn>> UFaction::GetPlayerList() const
{
	return PlayerList;
}

void UFaction::AddPlayer(const TObjectPtr<ACommanderPawn>& player)
{
	PlayerList.Add(player);
}

void UFaction::AddPlayers(const TArray<TObjectPtr<ACommanderPawn>>& newPlayerList)
{
	PlayerList.Append(newPlayerList);
}

bool UFaction::RemovePlayer(const TObjectPtr<ACommanderPawn>& player)
{
	return PlayerList.RemoveSingle(player) > 0;
}

bool UFaction::ContainsPlayer(const TObjectPtr<ACommanderPawn>& player)
{
	return PlayerList.Contains(player);
}

TArray<TObjectPtr<AAICommanderPawn>> UFaction::GetAIList() const
{
	return AIList;
}

void UFaction::AddAI(const TObjectPtr<AAICommanderPawn>& AI)
{
	AIList.Add(AI);
}

void UFaction::AddAIs(const TArray<TObjectPtr<AAICommanderPawn>>& newAIList)
{
	AIList.Append(newAIList);
}

bool UFaction::RemoveAI(const TObjectPtr<AAICommanderPawn>& AI)
{
	return AIList.RemoveSingle(AI) > 0;
}

bool UFaction::ContainsAI(const TObjectPtr<AAICommanderPawn>& AI)
{
	return AIList.Contains(AI);
}
