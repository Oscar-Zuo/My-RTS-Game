// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HUDInterface.h"
#include "GameFramework/HUD.h"
#include "DefaultHUD.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class RTS_API ADefaultHUD : public AHUD, public IHUDInterface
{
	GENERATED_BODY()
	
public:
	ADefaultHUD();

	void StartSelection_Implementation() override;
	void StopSelection_Implementation() override;
	void DrawHUD() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = States)
		FLinearColor selectionRectColor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = States)
		float selectionRectAlpha;
private:
	bool isDrawing;
	FVector2D pointA, pointB;
};
