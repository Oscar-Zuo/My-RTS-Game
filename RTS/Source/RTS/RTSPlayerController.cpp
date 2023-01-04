// Copyright Epic Games, Inc. All Rights Reserved.

#include "RTSPlayerController.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "RTSPlayerCameraManager.h"
#include "Pawn/BasicCharacter.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "AI/BasicAIController.h"
#include "HUD/DefaultHUD.h"
#include "InputState.h"
#include "Pawn/BasicCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

ARTSPlayerController::ARTSPlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;
	CachedDestination = FVector::ZeroVector;
	FollowTime = 0.f;
	PlayerCameraManagerClass = ARTSPlayerCameraManager::StaticClass();
}

void ARTSPlayerController::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Add Input Mapping Context
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(DefaultMappingContext, 0);
	}

	// set player pawn, normally should be commander pawn
	playerPawn = Cast<ACommanderPawn>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	SetViewTarget(playerPawn);

	// set units under control pointer
	unitsUnderControl = playerPawn->GetUnitsUnderCommand();
	unitsUnderControlForBlueprint = *unitsUnderControl;
}

void ARTSPlayerController::SetupInputComponent()
{
	// Set up gameplay key bindings
	Super::SetupInputComponent();
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent))
	{
		// Setup mouse input events
		// Select destination events
		EnhancedInputComponent->BindAction(SelectClickAction, ETriggerEvent::Started, this, &ARTSPlayerController::OnSelectClicked);
		EnhancedInputComponent->BindAction(SelectClickAction, ETriggerEvent::Started, this, &ARTSPlayerController::OnSelectDragging);
		EnhancedInputComponent->BindAction(SelectClickAction, ETriggerEvent::Completed, this, &ARTSPlayerController::OnSelectStopDragging);
		EnhancedInputComponent->BindAction(SelectClickAction, ETriggerEvent::Canceled, this, &ARTSPlayerController::OnSelectStopDragging);

		// Set Destination events
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Started, this, &ARTSPlayerController::OnInputStarted);
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Triggered, this, &ARTSPlayerController::OnSetDestinationTriggered);
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Completed, this, &ARTSPlayerController::OnSetDestinationReleased);
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Canceled, this, &ARTSPlayerController::OnSetDestinationReleased);
		EnhancedInputComponent->BindAction(ZoomCameraAction, ETriggerEvent::Triggered, this, &ARTSPlayerController::ZoomCamera);
		EnhancedInputComponent->BindAction(RotateCameraAction, ETriggerEvent::Triggered, this, &ARTSPlayerController::RotateCamera);
	}
}

void ARTSPlayerController::OnInputStarted()
{
	// get controlled pawn/unit
	for (auto& unit : *unitsUnderControl)
	{
		if (unit)
			unit->GetController()->StopMovement();
	}

}

// Triggered every frame when the input is held down
void ARTSPlayerController::OnSetDestinationTriggered()
{
	// We flag that the input is being pressed
	FollowTime += GetWorld()->GetDeltaSeconds();
	
	// We look for the location in the world where the player has pressed the input
	FHitResult Hit;
	bool bHitSuccessful = GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, true, Hit);


	// If we hit a surface, cache the location
	if (bHitSuccessful)
	{
		CachedDestination = Hit.Location;
	}

	// Move towards mouse pointer
	// TODO::Form a formation
	if (unitsUnderControl)
	{
		for (auto& unit : *unitsUnderControl)
		{
			if (unit)
			{
				FVector WorldDirection = (CachedDestination - unit->GetActorLocation()).GetSafeNormal();
				unit->AddMovementInput(WorldDirection, 1.0, false);
			}
		}
	}
}

void ARTSPlayerController::OnSetDestinationReleased()
{
	// If it was a short press
	if (FollowTime <= ShortPressThreshold && unitsUnderControl)
	{
		// We move there and spawn some particles
		for (auto& unit : *unitsUnderControl)
		{
			if (unit)
			{
				TObjectPtr<ABasicAIController> AIController = Cast<ABasicAIController>(unit->GetController());
				AIController->MoveToLocation(CachedDestination);
				UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, FXCursor, CachedDestination, FRotator::ZeroRotator, FVector(1.f, 1.f, 1.f), true, true, ENCPoolMethod::None, true);
			}
		}

		FollowTime = 0.f;
	}
}

void ARTSPlayerController::OnSelectClicked()
{
	FHitResult HitResult;
	GetHitResultUnderCursor(ECollisionChannel::ECC_Pawn, false, HitResult);

	TObjectPtr<APawn> selectedPawn = Cast<APawn>(HitResult.GetActor());
	if (selectedPawn)
	{
		// TODO:: Add more pawn types to control and replace the printf to HUD
		if (selectedPawn->Tags.Contains(FName("Character")))
		{
			GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Yellow, FString::Printf(TEXT("Mouse Click+++ Actor: %s"), *selectedPawn->GetName()));
			playerPawn->SetUnitsUnderCommand(TArray<TObjectPtr<APawn>>{selectedPawn});
		}
	}
	else
	{
		// clear unit under command if hit other object
		playerPawn->ClearUnitsUnderCommand();
	}
}

void ARTSPlayerController::OnSelectDragging()
{
	TObjectPtr<ADefaultHUD> hud = GetHUD<ADefaultHUD>();
	if (hud->Implements<UHUDInterface>())
		IHUDInterface::Execute_StartSelection(hud);
}

void ARTSPlayerController::OnSelectStopDragging()
{
	TObjectPtr<ADefaultHUD> hud = GetHUD<ADefaultHUD>();
	if (hud->Implements<UHUDInterface>())
		IHUDInterface::Execute_StopSelection(hud);
}

void ARTSPlayerController::ZoomCamera(const FInputActionInstance& Instance)
{
	float value = Instance.GetValue().Get<float>();
	playerPawn->ZoomCamera(value);
}

void ARTSPlayerController::RotateCamera(const FInputActionInstance& Instance)
{
	FVector2D value = Instance.GetValue().Get<FVector2D>();
	if (value.Y != 0 || value.X != 0)
	{
		playerPawn->RotateCamera(FRotator(value.Y, value.X, 0));
	}
}
