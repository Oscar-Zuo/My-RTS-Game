// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Squads/BasicSquad.h"
#include "Pawn/BasicCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "AI/Formations/ColumnFormation.h"
#include "AI/Formations/BasicFormation.h"
#include "BasicLevelScriptActor.h"
#include "CommanderPawn.h"
#include "DrawDebugHelpers.h"

ABasicSquad::ABasicSquad()
{
	DefaultFormation = UColumnFormation::StaticClass();
	CollisionEnabledHasPhysics(ECollisionEnabled::NoCollision);
}

void ABasicSquad::BeginPlay()
{
	Super::BeginPlay();

	// need to wait for commander pawn finishing beginplay function
	TObjectPtr< ACommanderPawn> playerPawn = Cast<ACommanderPawn>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	if (playerPawn && !playerPawn->HasActorBegunPlay())
	{
		playerPawn->DispatchBeginPlay();
	}

	// if no formation is setted for this squad, use the default formation
	if (!Formation)
	{
		TWeakObjectPtr< ABasicLevelScriptActor> LevelBlueprint = Cast<ABasicLevelScriptActor>(GetWorld()->GetLevelScriptActor());
		if (!LevelBlueprint.IsValid())
			return;
		auto FormationPtr = LevelBlueprint.Get()->AllFormationsMap.Find(DefaultFormation->GetFName());
		if (!FormationPtr)
			Formation = LevelBlueprint.Get()->AddFormation(DefaultFormation).Get();
		else
			Formation = *FormationPtr;
	}

	// spawn squad members if it is empty
	if (SquadMembers.Num() == 0)
	{
		SpawnAllSquadMembers();
	}

	// Set squad for squadMembers
	for (auto unit : SquadMembers)
	{
		if (unit && unit->Tags.Contains("Character"))
		{
			Cast<ABasicCharacter>(unit)->SetSquad(this);
		}
	}
}

void ABasicSquad::Tick(float DeltaSeconds)
{
}

void ABasicSquad::StopMovement()
{
	for (auto unit : SquadMembers)
	{
		if (!unit)
			continue;
		unit->GetController()->StopMovement();
	}
}

TObjectPtr<APawn> ABasicSquad::GetLeader() const
{
	return SquadMembers.IsEmpty() ? NULL : SquadMembers[0];
}

bool ABasicSquad::SetLeader(const TObjectPtr<APawn>& Leader)
{
	// switch new leader to new location
	if (SquadMembers.Contains(Leader))
		SquadMembers.Swap(0, SquadMembers.Find(Leader.Get()));
	else
		// return false when we can't find this pawn in the squad
		return false;
	return true;
}

void ABasicSquad::FindAndSwapLeader()
{
	// Maybe I need to change the squadMemebers to priority queue later? O(n) is not good enough
	int index = 0;
	for (auto unit : SquadMembers)
	{
		if (unit && unit->Tags.Contains(FName(TEXT("Leader"))))
		{
			SquadMembers.Swap(0, index);
			break;
		}
		index++;
	}
}

void ABasicSquad::ClearCommands()
{
	RemoveInvalidMembers();
	StopMovement();

	for (auto unit : SquadMembers)
	{
		// if it is a solider
		if (unit->Tags.Contains(FName(TEXT("Character"))))
		{
			TWeakObjectPtr<ABasicAIController> CharacterUnit = Cast<ABasicAIController>(unit->GetController());
			CharacterUnit->StopAndClearAllCommand();
		}
	}
}

void ABasicSquad::AddMovementInput(FVector WorldDirection, float ScaleValue, bool bForce)
{
	for (auto unit : SquadMembers)
	{
		if (unit)
			unit->AddMovementInput(WorldDirection, ScaleValue, bForce);
	}
}

void ABasicSquad::MoveToLocation(FVector Location, FVector2D direction)
{
	// remove all invaild members, we don't want a ghost in the squard
	RemoveInvalidMembers();
	FindAndSwapLeader();
	FVector leaderLocation = GetLeader()->GetActorLocation();
	
	TArray<FVector> locationsList = Formation->GetLocationList(SquadMembers.Num(), direction, FVector2D(Location.X, Location.Y));
	for (int i = 0; i < SquadMembers.Num(); i++)
	{
		auto unit = SquadMembers[i];
		// if it is a solider
		if (unit->Tags.Contains(FName(TEXT("Character"))))
		{
			unit = Cast<ABasicCharacter>(unit);

			// get bounding cylinder to get unit's size
			float collisionRadius, collisionHalfHeight;
			unit->GetComponentsBoundingCylinder(collisionRadius, collisionHalfHeight);
			Cast<ABasicAIController>(unit->GetController())->SendMoveToLocationCommand(locationsList[i]);
		}
	}
}

void ABasicSquad::AttackTarget(const TScriptInterface<IAttackableInterface>& Target)
{
	RemoveInvalidMembers();
	FindAndSwapLeader();

	for (auto unit: SquadMembers)
	{
		TWeakObjectPtr<ABasicCharacter> unitCharacter = Cast<ABasicCharacter>(unit);
		TWeakObjectPtr<ABasicAIController> unitController = Cast<ABasicAIController>(unit->GetController());
		if (unitController->Implements<UAttackableInterface>())
			IAttackableInterface::Execute_ConfirmAndAttackTarget(unitController.Get(), Target.GetObject());
	}
}

void ABasicSquad::SpawnAllSquadMembers()
{
	FVector squadLocation = GetActorLocation();
	int totalDefaultSquadMembersNum = 0;
	for (auto unitsTypeNumPair : DefaultSquadMembersNum)
	{
		totalDefaultSquadMembersNum += unitsTypeNumPair.Value;
	}

	// Spawn squad members and set their locations

	struct FActorSpawnParameters spawnPara;
	spawnPara.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	for (auto unitsTypeNumPair : DefaultSquadMembersNum)
	{
		for (int i = 0; i < unitsTypeNumPair.Value; i++)
		{
			TObjectPtr<ABasicCharacter> newUnit = GetWorld()->SpawnActor<ABasicCharacter>(unitsTypeNumPair.Key, spawnPara);
			SquadMembers.Add(newUnit);
		}
	}
	// remember to assign a leader
	FindAndSwapLeader();

	//Get locations for every squad member
	TArray<float> collisionsRadiusList, collisionsHalfHeightList;
	for (auto unit: SquadMembers)
	{
		// get bounding cylinder to get unit's size
		float collisionRadius, collisionHalfHeight;
		unit->GetComponentsBoundingCylinder(collisionRadius, collisionHalfHeight);
		collisionsRadiusList.Add(collisionRadius);
		collisionsHalfHeightList.Add(collisionHalfHeight);
	}
	auto locationsList = Formation->GetLocationList(totalDefaultSquadMembersNum, FVector2D(1,0), FVector2D(squadLocation.X, squadLocation.Y), collisionsRadiusList);

	for (int i = 0; i < SquadMembers.Num(); i++)
	{
		DrawDebugLine(GetWorld(), locationsList[i] + FVector(0, 0, 100), locationsList[i], FColor::Green);
		SquadMembers[i]->SetActorLocationAndRotation(locationsList[i], GetActorRotation());
	}
}

void ABasicSquad::RemoveInvalidMembers()
{
	for (auto unit : SquadMembers)
	{
		if (!unit || !unit->Controller)
			SquadMembers.Remove(unit);
	}
	DestorySquadIfEmpty();
}

TArray<TObjectPtr<APawn>> ABasicSquad::GetSquadMemebers() const
{
	return SquadMembers;
}

void ABasicSquad::SetSquadMembers(const TArray<TObjectPtr<APawn>>& MemberList)
{
	SquadMembers.Empty();
	for (auto unit : MemberList)
	{
		// set squad for every unit
		AddSquadMember(unit);
	}
	DestorySquadIfEmpty();
}

void ABasicSquad::AddSquadMember(TObjectPtr<APawn> Unit)
{
	if (Unit->Tags.Contains(FName(TEXT("Character"))))
	{
		Cast<ABasicCharacter>(Unit)->SetSquad(this);
		if (Unit->Tags.Contains(FName(TEXT("Leader"))))
			SquadMembers.Insert(Unit, 0);
		else
			SquadMembers.Add(Unit);
	}
}

bool ABasicSquad::IsSquadMembersContain(TObjectPtr<APawn> Unit) const
{
	return SquadMembers.Contains(Unit);
}

bool ABasicSquad::RemoveSquadMember(TObjectPtr<APawn> Unit)
{
	if (SquadMembers.Contains(Unit))
	{
		SquadMembers.Remove(Unit);
		DestorySquadIfEmpty();
		return true;
	}
	return false;
}

bool ABasicSquad::DestorySquadIfEmpty()
{
	if (SquadMembers.IsEmpty())
	{
		Destroy();
		// ask for memeory recycle
		MarkAsGarbage();
	}
	return false;
}

TWeakObjectPtr<UBasicFormation> ABasicSquad::GetFormation() const
{
	return Formation;
}

void ABasicSquad::SetFormation(const TWeakObjectPtr<UBasicFormation>& NewFormation)
{
	if (NewFormation.IsValid())
		Formation = NewFormation.Get();
}

bool ABasicSquad::CanBeSelectedByCommander(TWeakObjectPtr<ACommanderPawn> TargetCommander)
{
	if (!TargetCommander.IsValid())
		return false;
	return TargetCommander.Get() == SquadOwner.Get();
}
