// Fill out your copyright notice in the Description page of Project Settings.


#include "Pawn/AttackableInterface.h"

// Add default functionality here for any IAttackableInterface functions that are not pure virtual.

bool IAttackableInterface::CanBeAttacked_Implementation(TWeakObjectPtr<AActor> Attacker)
{
	return false;
}

TEnumAsByte<EAttackResult> IAttackableInterface::ReceiveDamage_Implementation(TWeakObjectPtr<AActor> Attacker, float Damage)
{
	return EAttackResult::FAILED;
}
