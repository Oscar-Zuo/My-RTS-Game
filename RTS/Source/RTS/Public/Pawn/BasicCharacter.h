// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputState.h"
#include "BasicCharacter.generated.h"

/**
 * 
 */
UCLASS()
class RTS_API ABasicCharacter : public ACharacter
{
	GENERATED_BODY()
	
public:
	ABasicCharacter();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

};
