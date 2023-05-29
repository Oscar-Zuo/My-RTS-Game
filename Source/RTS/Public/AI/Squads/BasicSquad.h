// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CommanderPawn.h"
#include "BasicSquad.generated.h"

/**
 * 
 */

class UBasicFormation;

UCLASS(Blueprintable)
class RTS_API ABasicSquad : public AActor
{
	GENERATED_BODY()
	
protected:
	// the first index(0) is the squad leader pointer
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Status)
		TArray<TObjectPtr<APawn>> SquadMembers;

	// maps bewteen units types and units number
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Status)
		TMap<TSubclassOf<APawn>, int> DefaultSquadMembersNum;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Status)
		TObjectPtr<UBasicFormation> Formation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Status)
		TSubclassOf<UBasicFormation> DefaultFormation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Status)
		TObjectPtr<ACommanderPawn> SquadOwner;

public:
	ABasicSquad();

	FORCEINLINE void StopMovement();

	// setter getter for learder
	FORCEINLINE TObjectPtr<APawn> GetLeader() const;
	FORCEINLINE bool SetLeader(const TObjectPtr<APawn>& _learder);
	
	// swap the leader to first if possible
	FORCEINLINE void FindAndSwapLeader();

	// Movement functions for every squad members
	void AddMovementInput(FVector WorldDirection, float ScaleValue, bool bForce = false);
	void MoveToLocation(FVector Location, FVector2D direction);

	//Spawn All Squad Members, only use it when spawning the squad
	void SpawnAllSquadMembers();

	FORCEINLINE void RemoveInvalidMembers();

	// squad members functions
	FORCEINLINE TArray<TObjectPtr< APawn >> GetSquadMemebers() const;
	FORCEINLINE void SetSquadMembers(const TArray<TObjectPtr< APawn >>& _squad);
	FORCEINLINE void AddSquadMember(TObjectPtr<APawn> _unit);
	FORCEINLINE bool IsSquadMembersContain(TObjectPtr< APawn > _unit) const;
	FORCEINLINE bool RemoveSquadMember(TObjectPtr<APawn> _unit);
	FORCEINLINE bool DestorySquadIfEmpty();
	
	// formation getter setter
	FORCEINLINE TWeakObjectPtr< UBasicFormation> GetFormation() const;
	FORCEINLINE void SetFormation(const TWeakObjectPtr< UBasicFormation>& _formation);

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
};
