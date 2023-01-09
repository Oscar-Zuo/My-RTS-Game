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
#include "AI/Squads/BasicSquad.h"
#include "AI/Formations/ColumnFormation.h"
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
	squadsUnderControl = playerPawn->GetSquadsUnderCommand();
	unitsUnderControlForBlueprint = *squadsUnderControl;
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
		EnhancedInputComponent->BindAction(SelectDragAction, ETriggerEvent::Started, this, &ARTSPlayerController::OnSelectDragging);
		EnhancedInputComponent->BindAction(SelectDragAction, ETriggerEvent::Completed, this, &ARTSPlayerController::OnSelectStopDragging);
		EnhancedInputComponent->BindAction(SelectDragAction, ETriggerEvent::Canceled, this, &ARTSPlayerController::OnSelectStopDragging);

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
	for (auto& squad : *squadsUnderControl)
	{
		if (squad)
			squad->StopMovement();
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
	if (squadsUnderControl)
	{
		for (auto& squad : *squadsUnderControl)
		{
			if (squad)
			{
				FVector WorldDirection = (CachedDestination - squad->GetLeader()->GetActorLocation()).GetSafeNormal();
				squad->AddMovementInput(WorldDirection, 1.0, false);
			}
		}
	}
}

void ARTSPlayerController::OnSetDestinationReleased()
{
	// If it was a short press
	if (FollowTime <= ShortPressThreshold && squadsUnderControl)
	{
		// We move there and spawn some particles
		for (auto& squad : *squadsUnderControl)
		{
			if (squad)
			{
				// TODO::Direction need to be decided by player input

				float direction = 0;
				squad->MoveToLocation(CachedDestination, 0);
			}
			else
			{
				playerPawn->ClearSquadsUnderCommand();
			}
		}
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, FXCursor, CachedDestination, FRotator::ZeroRotator, FVector(1.f, 1.f, 1.f), true, true, ENCPoolMethod::None, true);
	}
	FollowTime = 0.f;
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
			playerPawn->SetSquadsUnderCommand(TArray<TObjectPtr<ABasicSquad>>{Cast<ABasicCharacter>(selectedPawn)->GetSquad()});
		}
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


// camera control functions
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
