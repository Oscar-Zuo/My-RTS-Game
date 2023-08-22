// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Faction.generated.h"

/**
 * 
 */

UENUM(BlueprintType)
enum EFactionRelationship
{
	kFriendly		UMETA(DisplayName = "Friendly"),
	kHostile		UMETA(DisplayName = "Hostile"),
};


class ACommanderPawn;
class AAICommanderPawn;

UCLASS(BlueprintType)
class RTS_API UFaction : public UObject
{
	GENERATED_BODY()
	
public:
	TEnumAsByte<EFactionRelationship> DefaultRelationship;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Name;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<TObjectPtr<UFaction>, TEnumAsByte<EFactionRelationship>> RelationshipMap;
	TArray<TWeakObjectPtr<ACommanderPawn>> PlayerList;
	TArray<TWeakObjectPtr<AAICommanderPawn>> AIList;

public:
	UFaction();

	FORCEINLINE TArray<TWeakObjectPtr<ACommanderPawn>> GetPlayerList() const;
	FORCEINLINE void AddPlayer(const TWeakObjectPtr<ACommanderPawn>& player);
	FORCEINLINE void AddPlayers(const TArray<TWeakObjectPtr<ACommanderPawn>>& newPlayerList);
	FORCEINLINE bool RemovePlayer(const TWeakObjectPtr<ACommanderPawn>& player);
	FORCEINLINE bool ContainsPlayer(const TWeakObjectPtr<ACommanderPawn>& player);
	FORCEINLINE TArray<TWeakObjectPtr<AAICommanderPawn>> GetAIList() const;
	FORCEINLINE void AddAI(const TWeakObjectPtr<AAICommanderPawn>& AI);
	FORCEINLINE void AddAIs(const TArray<TWeakObjectPtr<AAICommanderPawn>>& newAIList);
	FORCEINLINE bool RemoveAI(const TWeakObjectPtr<AAICommanderPawn>& AI);
	FORCEINLINE bool ContainsAI(const TWeakObjectPtr<AAICommanderPawn>& AI);
	FORCEINLINE TEnumAsByte<EFactionRelationship> GetRelationshipBetweenFactions(TWeakObjectPtr<UFaction> TargetFaction);
};
