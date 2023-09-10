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
#include "GameSettings/Faction.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

ARTSPlayerController::ARTSPlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;
	cachedLoction = FVector::ZeroVector;
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
	OwnerCommandePawn = Cast<ACommanderPawn>(GetPawn());
	SetViewTarget(OwnerCommandePawn.Get());
}

void ARTSPlayerController::SetupInputComponent()
{
	// Set up gameplay key bindings
	Super::SetupInputComponent();
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent))
	{
		// Setup mouse input events
		// Select Actors events
		EnhancedInputComponent->BindAction(SelectClickAction, ETriggerEvent::Started, this, &ARTSPlayerController::OnSelectClicked);
		EnhancedInputComponent->BindAction(SelectDragAction, ETriggerEvent::Started, this, &ARTSPlayerController::OnSelectDragging);
		EnhancedInputComponent->BindAction(SelectDragAction, ETriggerEvent::Completed, this, &ARTSPlayerController::OnSelectStopDragging);
		EnhancedInputComponent->BindAction(SelectDragAction, ETriggerEvent::Canceled, this, &ARTSPlayerController::OnSelectStopDragging);

		// Set Destination events
		EnhancedInputComponent->BindAction(RightKeyClickAction, ETriggerEvent::Started, this, &ARTSPlayerController::OnRightKeyInputStarted);
		EnhancedInputComponent->BindAction(RightKeyClickAction, ETriggerEvent::Triggered, this, &ARTSPlayerController::OnRightKeyInputTriggered);
		EnhancedInputComponent->BindAction(RightKeyClickAction, ETriggerEvent::Completed, this, &ARTSPlayerController::OnRightKeyInputReleased);
		EnhancedInputComponent->BindAction(RightKeyClickAction, ETriggerEvent::Canceled, this, &ARTSPlayerController::OnRightKeyInputReleased);
		EnhancedInputComponent->BindAction(ZoomCameraAction, ETriggerEvent::Triggered, this, &ARTSPlayerController::ZoomCamera);
		EnhancedInputComponent->BindAction(RotateCameraAction, ETriggerEvent::Triggered, this, &ARTSPlayerController::RotateCamera);
	}
}

TWeakObjectPtr<ABasicCharacter> ARTSPlayerController::TryGetEnemyUnderCursor()
{
	// Get first clicked location
	FHitResult Hit;

	// If we hit a solider, perform attack or follow or something else
	if (!GetHitResultUnderCursor(ECollisionChannel::ECC_Pawn, true, Hit))
		return NULL;

	TWeakObjectPtr<ABasicCharacter> HitActor = Cast<ABasicCharacter>(Hit.GetActor());
	// Be sure the actor is a hostile
	if (HitActor.IsValid() && OwnerCommandePawn->Faction->GetRelationshipBetweenFactions(HitActor->GetFaction()) == EFactionRelationship::kHostile)
		return HitActor;
	return NULL;
}

void ARTSPlayerController::OnRightKeyInputStarted()
{
	// get controlled squad
	for (auto& Squad : OwnerCommandePawn->SquadsUnderCommand)
	{
		// clear all the commmands
		if (Squad)
			Squad->ClearCommands();
	}

	FHitResult Hit;

	TScriptInterface<IAttackableInterface> Enemy{TryGetEnemyUnderCursor().Get()};
	
	// Attack has prority to move
	if (Enemy.GetObject())
	{
		OnAttackInputVerified(Enemy);
	}
	// If we hit a surface, cache the location for later use
	else if (GetHitResultUnderCursor(ECollisionChannel::ECC_WorldStatic, true, Hit))
	{
		FirstMoveClickedLocation = Hit.Location;
	} 
}

// Triggered every frame when the input is held down
void ARTSPlayerController::OnRightKeyInputTriggered()
{
	// We look for the location in the world where the player has pressed the input
	FHitResult Hit;

	// If we hit a surface, cache the location
	if (GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, true, Hit))
	{
		cachedLoction = Hit.Location;
	}

	// Move towards mouse pointer
	// TODO::Form a formation
	if (!OwnerCommandePawn->SquadsUnderCommand.IsEmpty())
	{
		for (auto& Squad : OwnerCommandePawn->SquadsUnderCommand)
		{
			if (Squad) continue;

			FVector WorldDirection = (cachedLoction - Squad->GetLeader()->GetActorLocation()).GetSafeNormal();
			Squad->AddMovementInput(WorldDirection, 1.0, false);
		}
	}
}

void ARTSPlayerController::OnRightKeyInputReleased()
{
	if (OwnerCommandePawn->SquadsUnderCommand.IsEmpty())
		return;

	// Get direction vector
	FVector2D from = FVector2D(FirstMoveClickedLocation.X, FirstMoveClickedLocation.Y);
	FVector2D to = FVector2D(cachedLoction.X, cachedLoction.Y);
	FVector2D direction = (to - from).GetSafeNormal();

	for (auto& Squad : OwnerCommandePawn->SquadsUnderCommand)
	{
		if (!Squad)
		{
			OwnerCommandePawn->SquadsUnderCommand.Remove(Squad);
			continue;
		}
			
		if (FVector2D::Distance(from, to) <= 10)  // Judge if player want squad facing certain direction
		{
			FVector LeaderLocation = Squad->GetLeader()->GetActorLocation();
			// If not, let the squad face the moving direction 
			direction = (to - FVector2D(LeaderLocation.X, LeaderLocation.Y)).GetSafeNormal();
		}
		Squad->MoveToLocation(FirstMoveClickedLocation, direction);
	}

	// Spawn Cursor
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, FXCursor, FirstMoveClickedLocation, FRotator::ZeroRotator, FVector(1.f, 1.f, 1.f), true, true, ENCPoolMethod::None, true);
}

void ARTSPlayerController::OnSelectClicked()
{
	FHitResult HitResult;
	GetHitResultUnderCursor(ECollisionChannel::ECC_Pawn, false, HitResult);

	TWeakObjectPtr<APawn> selectedPawn = Cast<APawn>(HitResult.GetActor());
	if (!selectedPawn.IsValid())
		return;

	// TODO:: Add more pawn types to control and replace the printf to HUD
	if (selectedPawn.Get()->Tags.Contains(FName("Character")))
	{
		auto SelectedCharacter = Cast<ABasicCharacter>(selectedPawn.Get());
		if (SelectedCharacter->GetSquad()->CanBeSelectedByCommander(OwnerCommandePawn))
		{
			GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Yellow, FString::Printf(TEXT("Mouse Click Actor: %s"), *selectedPawn->GetName()));
			OwnerCommandePawn->SquadsUnderCommand.Empty();
			OwnerCommandePawn->SquadsUnderCommand.Add(SelectedCharacter->GetSquad().Get());
		}
		else
			// Clear the squad if selected nothing
			OwnerCommandePawn->SquadsUnderCommand.Empty();
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
	OwnerCommandePawn->ZoomCamera(value);
}

void ARTSPlayerController::RotateCamera(const FInputActionInstance& Instance)
{
	FVector2D value = Instance.GetValue().Get<FVector2D>();
	if (value.Y != 0 || value.X != 0)
	{
		OwnerCommandePawn->RotateCamera(FRotator(value.Y, value.X, 0));
	}
}

void ARTSPlayerController::OnAttackInputVerified(const TScriptInterface<IAttackableInterface>& TargetCharacter)
{
	if (!TargetCharacter.GetObject())
		return;

	TWeakObjectPtr<ABasicCharacter> EnemyCharacterObject = Cast< ABasicCharacter>(TargetCharacter.GetObject());

	auto squads = OwnerCommandePawn->SquadsUnderCommand;

	for (auto squad : squads)
	{
		squad->AttackTarget(TargetCharacter);
	}
}
