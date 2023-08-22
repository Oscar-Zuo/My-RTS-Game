// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HUDInterface.h"
#include "GameFramework/HUD.h"
#include "DefaultHUD.generated.h"

/**
 * 
 */

class ABasicSquad;
class ACommanderPawn;

UCLASS(Blueprintable)
class RTS_API ADefaultHUD : public AHUD, public IHUDInterface
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = States)
		FLinearColor selectionRectColor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = States)
		float selectionRectAlpha;
private:
	bool bIsDrawing, bHasSelectionStopped;
	// PointA: coordinate of cursor when start draging
	FVector2D pointA, StopPosition;
	TWeakObjectPtr<ACommanderPawn> PlayerPawn;

public:
	ADefaultHUD();

	void StartSelection_Implementation() override;
	void StopSelection_Implementation() override;
	void DrawHUD() override;

protected:
	virtual void BeginPlay() override;

private:
	TArray< TWeakObjectPtr <ABasicSquad>> GetSelectedSquad();
	FORCEINLINE FVector2D GetCurrentCursorInScreenPosition() const;
};
