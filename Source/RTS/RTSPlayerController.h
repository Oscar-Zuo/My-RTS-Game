// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputActionValue.h"
#include "InputAction.h"
#include "Public/CommanderPawn.h"
#include "AI/Squads/BasicSquad.h"
#include "RTSPlayerController.generated.h"

/** Forward declaration to improve compiling times */
class UNiagaraSystem;

UCLASS()
class ARTSPlayerController : public APlayerController
{
	GENERATED_BODY()


public:
	ARTSPlayerController();

	/** Time Threshold to know if it was a short press */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	float ShortPressThreshold;

	/** FX Class that we will spawn when clicking */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UNiagaraSystem* FXCursor;

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputMappingContext* DefaultMappingContext;

	// Move Camera Action
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* ZoomCameraAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* RotateCameraAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* SetDestinationClickAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* SelectClickAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* SelectDragAction;

protected:
	/** True if the controlled character should navigate to the mouse cursor. */
	uint32 bMoveToMouseCursor : 1;

	virtual void SetupInputComponent() override;
	
	// To add mapping context
	virtual void BeginPlay() override;

	/** Input handlers for SetDestination action. */
	void OnInputStarted();
	void OnSetDestinationTriggered();
	void OnSetDestinationReleased();

	// Input handlers for Select action.
	void OnSelectClicked();
	void OnSelectDragging();
	void OnSelectStopDragging();

	// Input handlers for Camera movement action
	void ZoomCamera(const FInputActionInstance& Instance);
	void RotateCamera(const FInputActionInstance& Instance);
	
	// Controlled Pawns
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Debug)
	TArray<TObjectPtr<ABasicSquad>>* squadsUnderControl;

	// sadly blueprint doesn't accept a pointer to TArray
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Debug)
	TArray<TObjectPtr<ABasicSquad>> unitsUnderControlForBlueprint;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Status)
		int playerID;

private:
	FVector cachedLoction, firstClickedLocation;
	TObjectPtr<ACommanderPawn> playerPawn;
};


