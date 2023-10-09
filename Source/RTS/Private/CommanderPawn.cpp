// Fill out your copyright notice in the Description page of Project Settings.


#include "CommanderPawn.h"
#include <Kismet/GameplayStatics.h>
#include <Kismet/KismetMathLibrary.h>
#include <Math/UnrealMathUtility.h>
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "AI/Squads/BasicSquad.h"
#include "AI/Formations/BasicFormation.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "BasicLevelScriptActor.h"

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
	PawnSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("CenterSphere"));
	// Create camera boom
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));

	SpringArmComp->SetupAttachment(PawnSceneComponent);

	//Setting default properties of the SpringArmComp
	SpringArmComp->bUsePawnControlRotation = false;
	SpringArmComp->bEnableCameraLag = true;
	MaximumSpringArmLength = 600.0f;
	MinimumSpringArmLength = 0;
	SpringArmComp->TargetArmLength = (MaximumSpringArmLength + MinimumSpringArmLength) / 2;
	CameraRotator = FRotator(-60, 0, 0);
	SpringArmComp->SetRelativeRotation(CameraRotator);

	MainCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("MainCamera"));
	//Attach the Camera to the SpringArmComponent
	MainCameraComponent->AttachToComponent(SpringArmComp, FAttachmentTransformRules::KeepRelativeTransform);

	MoveCameraScreenBoundary = FVector2D(50, 50);

	//set camera movement speed
	CameraMoveSpeed = 1000.0f;
	CameraZoomingSpeed = 30.0f;
	CameraRotateSpeed = 1.5;

	// Nothing selected yet, clear the array
	SquadsUnderCommand.Empty();

	Tags.Add(FName(TEXT("Player")));
}

void ACommanderPawn::SpawnSquard(TSubclassOf<ABasicSquad> SquadType, FVector Location, FRotator Rotation)
{
	struct FActorSpawnParameters SpawnPara;
	SpawnPara.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	
	TObjectPtr<ABasicSquad> NewSquad = GetWorld()->SpawnActor<ABasicSquad>(SquadType, Location, Rotation, SpawnPara);
	NewSquad->SquadOwner = this;
	OwnedSquadList.Add(NewSquad);
}

// Called when the game starts or when spawned
void ACommanderPawn::BeginPlay()
{
	Super::BeginPlay();

	TObjectPtr<APlayerController> playerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
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

	// If cursor is reached or inside boundary of screen, move camera

	//Up
	if (axisValue.X <= MoveCameraScreenBoundary.X)
	{
		resultX = -UKismetMathLibrary::NormalizeToRange(MoveCameraScreenBoundary.X - axisValue.X, 0, MoveCameraScreenBoundary.X);
	}
	//Down
	else if (axisValue.X >= viewportSize.X - MoveCameraScreenBoundary.X)
	{
		resultX = UKismetMathLibrary::NormalizeToRange(MoveCameraScreenBoundary.X - viewportSize.X + axisValue.X, 0, MoveCameraScreenBoundary.X);
	}

	//Left
	if (axisValue.Y <= MoveCameraScreenBoundary.Y)
	{
		resultY = UKismetMathLibrary::NormalizeToRange(MoveCameraScreenBoundary.Y - axisValue.Y, 0, MoveCameraScreenBoundary.Y);
	}
	//Right
	else if(axisValue.Y >= viewportSize.Y - MoveCameraScreenBoundary.Y)
	{
		resultY = -UKismetMathLibrary::NormalizeToRange(MoveCameraScreenBoundary.Y - viewportSize.Y + axisValue.Y, 0, MoveCameraScreenBoundary.Y);
	}

	// reverse X&Y due to  Unreal axis
	FVector offset = FVector(resultY, resultX, 0);
	AddActorLocalOffset(offset * CameraMoveSpeed* DeltaTime);
}


// Rotate spring arm to rotate camera
void ACommanderPawn::RotateCamera(FRotator value)
{
	FRotator cameraRotation = SpringArmComp->GetRelativeRotation()+ value;
	cameraRotation = FRotator(UKismetMathLibrary::FClamp(cameraRotation.Pitch, -89, 0),
		cameraRotation.Yaw, cameraRotation.Roll);
	SpringArmComp->SetRelativeRotation(cameraRotation);
}

// Change length of spring arm to zoom camera
void ACommanderPawn::ZoomCamera(float value)
{
	SpringArmComp->TargetArmLength = UKismetMathLibrary::FClamp(SpringArmComp->TargetArmLength - value * CameraZoomingSpeed, MinimumSpringArmLength, MaximumSpringArmLength);
}
