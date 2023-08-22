// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/DefaultHUD.h"
#include  "Blueprint/WidgetLayoutLibrary.h"
#include "CommanderPawn.h"
#include "Kismet/GameplayStatics.h"
#include "Pawn/BasicCharacter.h"
#include "AI/Squads/BasicSquad.h"

ADefaultHUD::ADefaultHUD()
{
	selectionRectAlpha = 0.2;
	selectionRectColor = FLinearColor::Blue;
	selectionRectColor.A = selectionRectAlpha;
	bIsDrawing = false;
	bHasSelectionStopped = true;
}

void ADefaultHUD::StartSelection_Implementation()
{
	pointA = GetCurrentCursorInScreenPosition();
	bIsDrawing = true;
	bHasSelectionStopped = false;
}

void ADefaultHUD::StopSelection_Implementation()
{
	bHasSelectionStopped = true;
	StopPosition = GetCurrentCursorInScreenPosition();
}

void ADefaultHUD::DrawHUD()
{
	Super::DrawHUD();
	if (!bIsDrawing)
		return;
	// PointB: coordinate of cursor when function calling
	auto pointB = GetCurrentCursorInScreenPosition();
	//GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Yellow, FString::Printf(TEXT("A: %f %f    B:%f %f"), pointA.X, pointA.X, pointB.X, pointB.Y));

	if (FVector2D::Distance(pointA, pointB) < 10)
		return;
	DrawRect(selectionRectColor, pointA.X, pointA.Y, pointB.X - pointA.X, pointB.Y - pointA.Y);

	// if the draging has stoped, get the squard under the rect
	if (bHasSelectionStopped)
	{
		bIsDrawing = false;
		auto CommanderObject = Cast<ACommanderPawn>(GetOwningPawn());
		CommanderObject->SquadsUnderCommand.Empty();

		for (auto squad:GetSelectedSquad())
			CommanderObject->SquadsUnderCommand.Add(squad.Get());
	}
}

void ADefaultHUD::BeginPlay()
{
	PlayerPawn = Cast<ACommanderPawn>(PlayerOwner->GetPawn());
}

TArray<TWeakObjectPtr<ABasicSquad>> ADefaultHUD::GetSelectedSquad()
{
	// get selected pawns
	TArray<APawn*> selectedPawns;
	TArray< TWeakObjectPtr <ABasicSquad>> selectedSquad;
	GetActorsInSelectionRectangle<APawn>(pointA, StopPosition, selectedPawns);
	for (auto& pawn : selectedPawns)
	{
		if (!pawn->Tags.Contains(FName("Character")))
			continue;

		auto Squad = Cast<ABasicCharacter>(pawn)->GetSquad();
		// Add squad to list if it is not in there and can be selected
		if (Squad.IsValid() && !selectedSquad.Contains(Squad) && Squad->CanBeSelectedByCommander(PlayerPawn))
			selectedSquad.Add(Squad);
	}
	return selectedSquad;
}

FVector2D ADefaultHUD::GetCurrentCursorInScreenPosition() const
{
	return UWidgetLayoutLibrary::GetMousePositionOnViewport(GetWorld()) * UWidgetLayoutLibrary::GetViewportScale(GetWorld());;
}

