// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "CommanderPawn.generated.h"

class USpringArmComponent;
class UCameraComponent;
class ABasicPawn;
class UBasicFormation;
class ABasicSquad;
class UFaction;

UCLASS()
class RTS_API ACommanderPawn : public APawn
{
	GENERATED_BODY()

public:	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TObjectPtr<USceneComponent> PawnCenterSphere;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TObjectPtr<USceneComponent> PawnSceneComponent;

	// camera boom
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		TObjectPtr<USpringArmComponent> SpringArmComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Status)
		TWeakObjectPtr<UFaction> Faction;

public:
	// Sets default values for this pawn's properties
	ACommanderPawn();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// camera control function
	void MoveCamera(float DeltaTime);
	void RotateCamera(FRotator value);
	void ZoomCamera(float value);

	FORCEINLINE TObjectPtr<UCameraComponent> GetTopDownCameraComponent() const { return MainCameraComponent; }

	// unit under command setter getter
	FORCEINLINE void SetSquadsUnderCommand(const TArray<TObjectPtr<ABasicSquad>>& _unitUnderCommand);
	FORCEINLINE TArray<TObjectPtr<ABasicSquad>>* GetSquadsUnderCommand();
	FORCEINLINE void ClearSquadsUnderCommand();

protected:

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		TObjectPtr<UCameraComponent> MainCameraComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BasicSettings)
		FVector2D MoveCameraScreenBoundary;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BasicSettings)
		float CameraMoveSpeed;   // every second

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BasicSettings)
		float CameraZoomingSpeed;  // every trigger

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BasicSettings)
		float CameraRotateSpeed;  // degree every pixel

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BasicSettings)
		FRotator CameraRotator;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BasicSettings)
		float MaximumSpringArmLength;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BasicSettings)
		float MinimumSpringArmLength;

	// squards under command
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = GameStatus)
		TArray<TObjectPtr<ABasicSquad>> SquadsUnderCommand;
};
