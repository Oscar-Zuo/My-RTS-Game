// Fill out your copyright notice in the Description page of Project Settings.


#include "Pawn/AttackableInterface.h"

// Add default functionality here for any IAttackableInterface functions that are not pure virtual.

bool IAttackableInterface::CanAttack_Implementation(const TScriptInterface<IAttackableInterface>& Attacker)
{
	return false;
}

bool IAttackableInterface::ConfirmAndAttackTarget_Implementation(const TScriptInterface<IAttackableInterface>& Target)
{
	return false;
}

void IAttackableInterface::AttackFeedBack_Implementation(EAttackResult Result, const TScriptInterface<IAttackableInterface>& TargetController)
{
}

void IAttackableInterface::AttackTargetKilled_Implementation(const TScriptInterface<IAttackableInterface>& DyingTargetController)
{
}

void IAttackableInterface::EnableWeapons_Implementation()
{
}

void IAttackableInterface::DisableWeapons_Implementation()
{
}

bool IAttackableInterface::CanBeAttacked_Implementation(const TScriptInterface<IAttackableInterface>& Attacker, TSubclassOf<UDamageType> DamageType)
{
	return false;
}

void IAttackableInterface::SetAsAttacker_Implementation(const TScriptInterface<IAttackableInterface>& Attacker)
{
}

void IAttackableInterface::RemoveFromAttackers_Implementation(const TScriptInterface<IAttackableInterface>& Attacker)
{
}

float IAttackableInterface::TakeDamage_Implementation(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	return 0.0f;
}
