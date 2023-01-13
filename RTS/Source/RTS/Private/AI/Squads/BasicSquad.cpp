// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Squads/BasicSquad.h"
#include "Pawn/BasicCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "AI/Formations/ColumnFormation.h"
#include "AI/Formations/BasicFormation.h"
#include "CommanderPawn.h"

ABasicSquad::ABasicSquad()
{
	defaultFormation = UColumnFormation::StaticClass();
	CollisionEnabledHasPhysics(ECollisionEnabled::NoCollision);
}

void ABasicSquad::BeginPlay()
{
	Super::BeginPlay();

	// need to wait for commander pawn finishing beginplay function
	TObjectPtr< ACommanderPawn> playerPawn = Cast<ACommanderPawn>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	if (!playerPawn->HasActorBegunPlay())
	{
		playerPawn->DispatchBeginPlay();
	}

	// if no formation is setted for this squad, use the default formation
	if (!formation)
	{
		auto formationsMap = playerPawn->GetAllFormations();
		formation = formationsMap[defaultFormation->GetFName()];
		// TODO:: add match failure situation
	}

	// spawn squad members if it is empty
	if (squadMembers.Num() == 0)
	{
		SpawnAllSquadMembers();
	}

	// Set squad for squadMembers
	for (auto unit : squadMembers)
	{
		if (unit && unit->Tags.Contains("Character"))
		{
			Cast<ABasicCharacter>(unit)->SetSquad(this);
		}
	}
}

void ABasicSquad::Tick(float DeltaSeconds)
{
	if (GetLeader())
		SetActorLocationAndRotation(GetLeader()->GetActorLocation(), GetLeader()->GetActorRotation());
	else
		// if there is no one in the squad, just destory it
	{
		Destroy();
		// ask for memeory recycle
		MarkAsGarbage();
	}
}

void ABasicSquad::StopMovement()
{
	for (auto unit : squadMembers)
	{
		if (unit)
			unit->GetController()->StopMovement();
	}
}

TObjectPtr<APawn> ABasicSquad::GetLeader() const
{
	return squadMembers.IsEmpty() ? NULL : squadMembers[0];
}

bool ABasicSquad::SetLeader(const TObjectPtr<APawn>& _leader)
{
	// switch new leader to new location
	if (squadMembers.Contains(_leader))
		squadMembers.Swap(0, squadMembers.Find(_leader));
	else
		// return false when we can't find this pawn in the squad
		return false;
	return true;
}

void ABasicSquad::FindAndSwapLeader()
{
	// Maybe I need to change the squadMemebers to priority queue later? O(n) is not good enough
	int index = 0;
	for (auto unit : squadMembers)
	{
		if (unit && unit->Tags.Contains(FName(TEXT("Leader"))))
		{
			squadMembers.Swap(0, index);
			break;
		}
		index++;
	}
}

void ABasicSquad::AddMovementInput(FVector WorldDirection, float ScaleValue, bool bForce)
{
	for (auto unit : squadMembers)
	{
		if (unit)
			unit->AddMovementInput(WorldDirection, ScaleValue, bForce);
	}
}

void ABasicSquad::MoveToLocation(FVector Location, float direction)
{
	// remove all invaild members, we don't want a ghost in the squard
	RemoveInvalidMembers();
	FindAndSwapLeader();
	FVector leaderLocation = GetLeader()->GetActorLocation();
	TArray<FVector> locationsList = formation->GetLocationList(squadMembers.Num(), direction, FVector2D(Location.X, Location.Y));
	for (int i = 0; i < squadMembers.Num(); i++)
	{
		auto unit = squadMembers[i];
		// if it is a solider
		if (unit->Tags.Contains(FName(TEXT("Character"))))
		{
			unit = Cast<ABasicCharacter>(unit);

			// get bounding cylinder to get unit's size
			float collisionRadius, collisionHalfHeight;
			unit->GetComponentsBoundingCylinder(collisionRadius, collisionHalfHeight);
			Cast<ABasicAIController>(unit->GetController())->MoveToLocation(locationsList[i]);
		}
	}
}

void ABasicSquad::SpawnAllSquadMembers()
{
	FVector squadLocation = GetActorLocation();
	int totalDefaultSquadMembersNum = 0;
	for (auto unitsTypeNumPair : defaultSquadMembersNum)
	{
		totalDefaultSquadMembersNum += unitsTypeNumPair.Value;
	}

	// Spawn squad members and set their locations

	struct FActorSpawnParameters spawnPara;
	spawnPara.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	for (auto unitsTypeNumPair : defaultSquadMembersNum)
	{
		for (int i = 0; i < unitsTypeNumPair.Value; i++)
		{
			TObjectPtr<APawn> newUnit = GetWorld()->SpawnActor<APawn>(unitsTypeNumPair.Key, spawnPara);
			squadMembers.Add(newUnit);
		}
	}
	FindAndSwapLeader();

	//Get locations for every squad member
	TArray<float> collisionsRadiusList, collisionsHalfHeightList;
	for (auto unit: squadMembers)
	{
		// get bounding cylinder to get unit's size
		float collisionRadius, collisionHalfHeight;
		unit->GetComponentsBoundingCylinder(collisionRadius, collisionHalfHeight);
		collisionsRadiusList.Add(collisionRadius);
		collisionsHalfHeightList.Add(collisionHalfHeight);
	}
	auto locationsList = formation->GetLocationList(totalDefaultSquadMembersNum, 0, FVector2D(squadLocation.X, squadLocation.Y), collisionsRadiusList);


	for (int i = 0; i < squadMembers.Num(); i++)
	{
		squadMembers[i]->SetActorLocationAndRotation(locationsList[i], GetActorRotation());
	}
}

void ABasicSquad::RemoveInvalidMembers()
{
	for (auto unit : squadMembers)
	{
		if (!unit)
			squadMembers.Remove(unit);
	}
}

TArray<TObjectPtr<APawn>> ABasicSquad::GetSquadMemebers() const
{
	return squadMembers;
}

void ABasicSquad::SetSquadMembers(const TArray<TObjectPtr<APawn>>& _squad)
{
	squadMembers.Empty();
	for (auto unit : _squad)
	{
		// set squad for every unit
		AddSquadMember(unit);
	}
}

void ABasicSquad::AddSquadMember(TObjectPtr<APawn> _unit)
{
	if (_unit->Tags.Contains(FName(TEXT("Character"))))
	{
		Cast<ABasicCharacter>(_unit)->SetSquad(this);
		if (_unit->Tags.Contains(FName(TEXT("Leader"))))
			squadMembers.Insert(_unit, 0);
		else
			squadMembers.Add(_unit);
	}
}

bool ABasicSquad::IsSquadMembersContain(TObjectPtr<APawn> _unit) const
{
	return squadMembers.Contains(_unit);
}

bool ABasicSquad::RemoveSquadMember(TObjectPtr<APawn> _unit)
{
	if (squadMembers.Contains(_unit))
	{
		squadMembers.Remove(_unit);
		return true;
	}
	return false;
}

TObjectPtr<UBasicFormation> ABasicSquad::GetFormation() const
{
	return formation;
}

void ABasicSquad::SetFormation(const TObjectPtr<UBasicFormation>& _formation)
{
	formation = _formation;
}


