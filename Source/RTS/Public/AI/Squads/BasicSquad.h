// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CommanderPawn.h"
#include "Pawn/AttackableInterface.h"
#include "BasicSquad.generated.h"

/**
 * 
 */

class UBasicFormation;

UCLASS(Blueprintable)
class RTS_API ABasicSquad : public AActor
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Status)
	TObjectPtr<ACommanderPawn> SquadOwner;

protected:
	// the first index(0) is the squad leader pointer
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Status)
	TArray<TObjectPtr<APawn>> SquadMembers;

	// maps bewteen units types and units number
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Status, meta = (ExposeOnSpawn = "true"))
	TMap<TSubclassOf<APawn>, int> DefaultSquadMembersNum;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Status)
	TObjectPtr<UBasicFormation> Formation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Status, meta = (ExposeOnSpawn = "true"))
	TSubclassOf<UBasicFormation> DefaultFormation;

public:
	ABasicSquad();

	FORCEINLINE void StopMovement();

	// setter getter for learder
	FORCEINLINE TObjectPtr<APawn> GetLeader() const;
	FORCEINLINE bool SetLeader(const TObjectPtr<APawn>& Learder);

	//Spawn All Squad Members, only use it when spawning the squad
	void SpawnAllSquadMembers();

	FORCEINLINE void RemoveInvalidMembers();

	// squad members functions
	FORCEINLINE TArray<TObjectPtr<APawn>> GetSquadMemebers() const;
	FORCEINLINE void SetSquadMembers(const TArray<TObjectPtr<APawn>>& Squad);
	FORCEINLINE void AddSquadMember(TObjectPtr<APawn> Unit);
	FORCEINLINE bool IsSquadMembersContain(TObjectPtr<APawn> Unit) const;
	FORCEINLINE bool RemoveSquadMember(TObjectPtr<APawn> Unit);
	
	// formation getter setter
	FORCEINLINE TWeakObjectPtr< UBasicFormation> GetFormation() const;
	FORCEINLINE void SetFormation(const TWeakObjectPtr<UBasicFormation>& NewFormation);

	bool CanBeSelectedByCommander(TWeakObjectPtr<ACommanderPawn> TargetCommander);

	//------------------Command-------------------
	void ClearCommands();

	// Order functions
	// Movement functions 
	void MoveToLocation(FVector Location, FVector2D direction);
	void AddMovementInput(FVector WorldDirection, float ScaleValue, bool bForce = false);

	// Attack function
	void AttackTarget(const TScriptInterface<IAttackableInterface>& Target);

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	// swap the leader to first if possible
	FORCEINLINE void FindAndSwapLeader();
	FORCEINLINE bool DestorySquadIfEmpty();
};
