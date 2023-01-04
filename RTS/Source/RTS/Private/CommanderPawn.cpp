// Fill out your copyright notice in the Description page of Project Settings.


#include "CommanderPawn.h"
#include <Kismet/GameplayStatics.h>
#include <Kismet/KismetMathLibrary.h>
#include <Math/UnrealMathUtility.h>
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Pawn/BasicPawn.h"
#include "Blueprint/WidgetLayoutLibrary.h"

// Sets default values
ACommanderPawn::ACommanderPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Don't rotate character to camera direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Create pawn attachment scene component for camera boom
	pawnSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("CenterSphere"));
	// Create camera boom
	springArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));

	springArmComp->SetupAttachment(pawnSceneComponent);

	//Setting default properties of the SpringArmComp
	springArmComp->bUsePawnControlRotation = false;
	springArmComp->bEnableCameraLag = true;
	maximumSpringArmLength = 600.0f;
	minimumSpringArmLength = 0;
	springArmComp->TargetArmLength = (maximumSpringArmLength + minimumSpringArmLength) / 2;
	cameraRotator = FRotator(-60, 0, 0);
	springArmComp->SetRelativeRotation(cameraRotator);

	mainCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("MainCamera"));
	//Attach the Camera to the SpringArmComponent
	mainCameraComponent->AttachToComponent(springArmComp, FAttachmentTransformRules::KeepRelativeTransform);

	moveCameraScreenBoundary = FVector2D(50, 50);

	//set camera movement speed
	cameraMoveSpeed = 1000.0f;
	cameraZoomingSpeed = 30.0f;
	cameraRotateSpeed = 1.5;

	// Nothing selected yet, clear the array
	unitsUnderCommand.Empty();
}

void ACommanderPawn::SetUnitsUnderCommand(const TArray<TObjectPtr<APawn>>& _unitsUnderCommand)
{
	if (_unitsUnderCommand.IsEmpty())
		unitsUnderCommand.Empty();
	else
		unitsUnderCommand = _unitsUnderCommand;
}

TArray<TObjectPtr<APawn>>* ACommanderPawn::GetUnitsUnderCommand()
{
	return &unitsUnderCommand;
}

void ACommanderPawn::ClearUnitsUnderCommand()
{
	unitsUnderCommand.Empty();
}

// Called when the game starts or when spawned
void ACommanderPawn::BeginPlay()
{
	Super::BeginPlay();
	playerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
}

// Called every frame
void ACommanderPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	MoveCamera(DeltaTime);
}

void ACommanderPawn::MoveCamera(float DeltaTime)
{
	// coordinate of mouse cursor
	FVector2D axisValue = UWidgetLayoutLibrary::GetMousePositionOnViewport(GetWorld()) * UWidgetLayoutLibrary::GetViewportScale(GetWorld());
	// size of viewport
	FVector2D viewportSize = UWidgetLayoutLibrary::GetViewportSize(GetWorld());
	// maintain the mouse coordinate inside of the viewport size
	axisValue.Set(FMath::Clamp(axisValue.X, 0, viewportSize.X), FMath::Clamp(axisValue.Y, 0, viewportSize.Y));
	float resultX, resultY;

	// If cursor is reached or inside boundary of screen, then move camera
	// Get normalized direction vector
	//Up
	if (axisValue.X <= moveCameraScreenBoundary.X)
	{
		resultX = -UKismetMathLibrary::NormalizeToRange(moveCameraScreenBoundary.X - axisValue.X, 0, moveCameraScreenBoundary.X);
	}
	//Down
	else if (axisValue.X >= viewportSize.X - moveCameraScreenBoundary.X)
	{
		resultX = UKismetMathLibrary::NormalizeToRange(moveCameraScreenBoundary.X - viewportSize.X + axisValue.X, 0, moveCameraScreenBoundary.X);
	}

	//Left
	if (axisValue.Y <= moveCameraScreenBoundary.Y)
	{
		resultY = UKismetMathLibrary::NormalizeToRange(moveCameraScreenBoundary.Y - axisValue.Y, 0, moveCameraScreenBoundary.Y);
	}
	//Right
	else if(axisValue.Y >= viewportSize.Y - moveCameraScreenBoundary.Y)
	{
		resultY = -UKismetMathLibrary::NormalizeToRange(moveCameraScreenBoundary.Y - viewportSize.Y + axisValue.Y, 0, moveCameraScreenBoundary.Y);
	}

	// reverse X&Y due to  Unreal axis
	FVector offset = FVector(resultY, resultX, 0);
	AddActorLocalOffset(offset * cameraMoveSpeed* DeltaTime);
}


// Rotate spring arm to rotate camera
void ACommanderPawn::RotateCamera(FRotator value)
{
	FRotator cameraRotation = springArmComp->GetRelativeRotation()+ value;
	cameraRotation = FRotator(UKismetMathLibrary::FClamp(cameraRotation.Pitch, -89, 0),
		cameraRotation.Yaw, cameraRotation.Roll);
	springArmComp->SetRelativeRotation(cameraRotation);
}

// Change length of spring arm to zoom camera
void ACommanderPawn::ZoomCamera(float value)
{
	springArmComp->TargetArmLength = UKismetMathLibrary::FClamp(springArmComp->TargetArmLength - value * cameraZoomingSpeed, minimumSpringArmLength, maximumSpringArmLength);
}