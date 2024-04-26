// Fill out your copyright notice in the Description page of Project Settings.

#include "Pawn/BasicCharacter.h"
#include "AI/BasicAIController.h"
#include "AI/Squads/BasicSquad.h"
#include "GameSettings/Faction.h"
#include "Components/DecalComponent.h"
#include "Components/CapsuleComponent.h"
#include "NavModifierComponent.h"
#include "NavAreas/NavArea_LowHeight.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "GameFramework/PlayerController.h"


ABasicCharacter::ABasicCharacter()
{
	// Set size for player capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate character to camera direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Rotate character to moving direction
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	// Activate ticking in order to update the cursor every frame.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	// set tag for character
	Tags.Add(FName(TEXT("Character")));

	// Initialize character status
	IsAttacking = false;

	OnTakeAnyDamage.AddDynamic(this, &ABasicCharacter::OnReceiveDamage);
}

// Called when the game starts or when spawned
void ABasicCharacter::BeginPlay()
{
	Super::BeginPlay();
	// if there is no controller set
	if (!GetController())
	{
		SpawnDefaultController();
	}

	FogOfWarMaterialHiddenInstance = UMaterialInstanceDynamic::Create(FogOfWarHiddenMaterial, this);
	FogOfWarMaterialRevealInstance = UMaterialInstanceDynamic::Create(FogOfWarRevealMaterial, this);
	UKismetRenderingLibrary::ClearRenderTarget2D(this, FogOfWarHiddenRender, FColor::Black);
	UKismetRenderingLibrary::ClearRenderTarget2D(this, FogOfWarRevealRender, FColor::Black);
}

// Called every frame
void ABasicCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	ClearFogOfWar();
}

// Called to bind functionality to input
void ABasicCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ABasicCharacter::UpdateRevealRadius()
{
	TObjectPtr<ABasicAIController> controller = Cast<ABasicAIController>(Controller);
	if (!controller)
		return;
	FogOfWarMaterialRevealInstance->SetScalarParameterValue(FName(TEXT("Size")), controller->GetVisionRadius());
}

void ABasicCharacter::SetSquad(const TWeakObjectPtr<ABasicSquad>& _squad)
{
	Squad = _squad.Get();
}

TWeakObjectPtr<ABasicSquad> ABasicCharacter::GetSquad() const
{
	return Squad;
}

TWeakObjectPtr<UFaction> ABasicCharacter::GetFaction()
{
	return Squad->SquadOwner->Faction;
}

UFaction* ABasicCharacter::GetFaction_BP()
{
	return GetFaction().Get();
}

bool ABasicCharacter::ConfirmAndAttackTarget_Implementation(const TScriptInterface<IAttackableInterface>& Target)
{
	return GetController<ABasicAIController>()->ConfirmAndAttackTarget(Target);
}

bool ABasicCharacter::CanBeAttacked_Implementation(const TScriptInterface<IAttackableInterface>& Attacker, TSubclassOf<UDamageType> DamageType)
{
	return GetController<ABasicAIController>()->CanBeAttacked(Attacker, DamageType);
}

void ABasicCharacter::AttackFeedBack_Implementation(EAttackResult Result, const TScriptInterface<IAttackableInterface>& Target)
{
	GetController<ABasicAIController>()->AttackFeedBack(Result, Target);
}

void ABasicCharacter::AttackTargetKilled_Implementation(const TScriptInterface<IAttackableInterface>& DyingTarget)
{
	GetController<ABasicAIController>()->AttackTargetKilled(DyingTarget);
}

void ABasicCharacter::EnableWeapons_Implementation()
{
	GetController<ABasicAIController>()->EnableWeapons();
}

void ABasicCharacter::DisableWeapons_Implementation()
{
	GetController<ABasicAIController>()->DisableWeapons();
}

void ABasicCharacter::SetAsAttacker_Implementation(const TScriptInterface<IAttackableInterface>& Attacker)
{
	GetController<ABasicAIController>()->SetAsAttacker(Attacker);
}

void ABasicCharacter::RemoveFromAttackers_Implementation(const TScriptInterface<IAttackableInterface>& Attacker)
{
	GetController<ABasicAIController>()->RemoveFromAttackers(Attacker);
}

float ABasicCharacter::TakeDamage_Implementation(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	return ACharacter::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}

void ABasicCharacter::OnReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	GetController<ABasicAIController>()->OnReceiveDamage(DamagedActor, Damage, DamageType, InstigatedBy, DamageCauser);
}