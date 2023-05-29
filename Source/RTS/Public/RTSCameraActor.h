// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraActor.h"
#include "RTSCameraActor.generated.h"

/**
 * 
 */
UCLASS()
class RTS_API ARTSCameraActor : public ACameraActor
{
	GENERATED_BODY()
	
public:
	ARTSCameraActor();
	
protected:
	void MoveCamera(FVector2D direction);
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CameraMoveSpeed = 300.0f;

};
