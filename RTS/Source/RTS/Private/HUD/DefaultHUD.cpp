// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/DefaultHUD.h"
#include  "Blueprint/WidgetLayoutLibrary.h"
#include "CommanderPawn.h"
#include "Kismet/GameplayStatics.h"

ADefaultHUD::ADefaultHUD()
{
	selectionRectAlpha = 0.2;
	selectionRectColor = FLinearColor::Blue;
	selectionRectColor.A = selectionRectAlpha;
	isDrawing = false;
}

void ADefaultHUD::StartSelection_Implementation()
{
	// coordinate of mouse cursor
	pointA = UWidgetLayoutLibrary::GetMousePositionOnViewport(GetWorld()) * UWidgetLayoutLibrary::GetViewportScale(GetWorld());
	isDrawing = true;
}

void ADefaultHUD::StopSelection_Implementation()
{
	// get units under selection rect
	TArray<TObjectPtr<APawn>> selectedPawns, selectedUnits;
	GetActorsInSelectionRectangle<APawn>(pointA, pointB, selectedPawns);
	for (auto& pawn : selectedPawns)
	{
		if (pawn->Tags.Contains(FName("Character")))
			selectedUnits.Add(pawn);
	}

	Cast<ACommanderPawn>(GetOwningPawn())->SetUnitsUnderCommand(selectedUnits);

	isDrawing = false;
}

void ADefaultHUD::DrawHUD()
{
	if (isDrawing)
	{
		pointB = UWidgetLayoutLibrary::GetMousePositionOnViewport(GetWorld()) * UWidgetLayoutLibrary::GetViewportScale(GetWorld());
		DrawRect(selectionRectColor, pointA.X, pointA.Y, pointB.X - pointA.X, pointB.Y - pointA.Y);
	}
}



