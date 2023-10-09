// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputState.h"
#include "AI/BasicAIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "BasicCharacter.generated.h"

/**
 * 
 */

class ABasicSquad;
class UNavModifierComponent;
class UFaction;

UCLASS()
class RTS_API ABasicCharacter : public ACharacter
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadWrite)
	bool IsAttacking;

protected:
	// Pointer to its squad
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<ABasicSquad> Squad;

private:
	UPROPERTY(Category = Character, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAIPerceptionComponent> AIPresecptionComponent;

public:
	ABasicCharacter();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	FORCEINLINE void SetSquad(const TWeakObjectPtr<ABasicSquad>& _squad);
	FORCEINLINE TWeakObjectPtr<ABasicSquad> GetSquad() const;

	// Helper functions
	FORCEINLINE TWeakObjectPtr<UFaction> GetFaction();


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
