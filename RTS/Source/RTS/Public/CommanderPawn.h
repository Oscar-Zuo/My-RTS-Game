// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "CommanderPawn.generated.h"

class USpringArmComponent;
class UCameraComponent;
class ABasicPawn;

UCLASS()
class RTS_API ACommanderPawn : public APawn
{
	GENERATED_BODY()

public:	
	// Sets default values for this pawn's properties
	ACommanderPawn();

	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TObjectPtr<UStaticMeshComponent> pawnCenterSphere;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TObjectPtr<USceneComponent> pawnSceneComponent;

	// camera control function
	void MoveCamera(float DeltaTime);
	void RotateCamera(FRotator value);
	void ZoomCamera(float value);

	FORCEINLINE TObjectPtr<UCameraComponent> GetTopDownCameraComponent() const { return mainCameraComponent; }

	// camera boom
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<USpringArmComponent> springArmComp;

	// unit under command setter getter
	FORCEINLINE void SetUnitsUnderCommand(const TArray<TObjectPtr<APawn>>& _unitUnderCommand);
	FORCEINLINE TArray<TObjectPtr<APawn>>* GetUnitsUnderCommand();
	FORCEINLINE void ClearUnitsUnderCommand();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		TObjectPtr<UCameraComponent> mainCameraComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BasicSettings)
		FVector2D moveCameraScreenBoundary;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BasicSettings)
		float cameraMoveSpeed;   // every second

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BasicSettings)
		float cameraZoomingSpeed;  // every trigger

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BasicSettings)
		float cameraRotateSpeed;  // degree every pixel

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BasicSettings)
		FRotator cameraRotator;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BasicSettings)
		float maximumSpringArmLength;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BasicSettings)
		float minimumSpringArmLength;

	// the units under command
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = GameStatus)
		TArray<TObjectPtr<APawn>> unitsUnderCommand;

	TObjectPtr<APlayerController> playerController;
};
