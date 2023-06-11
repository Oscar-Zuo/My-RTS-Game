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

UCLASS()
class RTS_API UFaction : public UObject
{
	GENERATED_BODY()
	
public:
	TEnumAsByte<EFactionRelationship> DefaultRelationship;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Name;

protected:
	TMap<TObjectPtr<UFaction>, TEnumAsByte<EFactionRelationship>>  RelationshipMap;
	TArray<TObjectPtr<ACommanderPawn>> PlayerList;
	TArray<TObjectPtr<AAICommanderPawn>> AIList;

public:
	UFaction();

	FORCEINLINE TArray<TObjectPtr<ACommanderPawn>> GetPlayerList() const;
	FORCEINLINE void AddPlayer(const TObjectPtr<ACommanderPawn>& player);
	FORCEINLINE void AddPlayers(const TArray<TObjectPtr<ACommanderPawn>>& newPlayerList);
	FORCEINLINE bool RemovePlayer(const TObjectPtr<ACommanderPawn>& player);
	FORCEINLINE bool ContainsPlayer(const TObjectPtr<ACommanderPawn>& player);
	FORCEINLINE TArray<TObjectPtr<AAICommanderPawn>> GetAIList() const;
	FORCEINLINE void AddAI(const TObjectPtr<AAICommanderPawn>& AI);
	FORCEINLINE void AddAIs(const TArray<TObjectPtr<AAICommanderPawn>>& newAIList);
	FORCEINLINE bool RemoveAI(const TObjectPtr<AAICommanderPawn>& AI);
	FORCEINLINE bool ContainsAI(const TObjectPtr<AAICommanderPawn>& AI);
};
