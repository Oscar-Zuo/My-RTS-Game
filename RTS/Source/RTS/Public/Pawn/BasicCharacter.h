// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputState.h"
#include "AI/BasicAIController.h"
#include "BasicCharacter.generated.h"

/**
 * 
 */

class ABasicSquad;

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

	FORCEINLINE void SetSquad(const TObjectPtr<ABasicSquad>& _squad);
	FORCEINLINE TObjectPtr<ABasicSquad> GetSquad() const;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	// Use our AI controller;
	TSubclassOf<ABasicAIController> AIControllerClass;

	// Pointer to its squad
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TObjectPtr<ABasicSquad> squad;
};
