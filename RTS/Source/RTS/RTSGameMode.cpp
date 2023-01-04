// Copyright Epic Games, Inc. All Rights Reserved.

#include "RTSGameMode.h"
#include "RTSPlayerController.h"
#include "GameFramework/HUD.h"
#include "UObject/ConstructorHelpers.h"

ARTSGameMode::ARTSGameMode()
{
	// use our custom PlayerController class
	PlayerControllerClass = ARTSPlayerController::StaticClass();

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Blueprints/BP_CommanderPawn"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	// set default controller to our Blueprinted controller
	static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerBPClass(TEXT("/Game/TopDown/Blueprints/BP_TopDownPlayerController"));
	if(PlayerControllerBPClass.Class != NULL)
	{
		PlayerControllerClass = PlayerControllerBPClass.Class;
	}
	
	// set default HUD to Blueprinted HUD
	static ConstructorHelpers::FClassFinder<AHUD> HUDBPClass(TEXT("/Game/Blueprints/HUD/HUD_DefaultHUD"));
	if (HUDBPClass.Class != NULL)
	{
		HUDClass = HUDBPClass.Class;
	}

}