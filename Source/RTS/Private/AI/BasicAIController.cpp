// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BasicAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AI/Commands/MoveCommand.h"
#include "Pawn/BasicCharacter.h"
#include "AI/Tasks/BTTask_BasicTask.h"
#include "AI/Tasks/BTTask_Attack.h"
#include "Weapons/DamageType_RegularBullet.h"
#include "Engine/DamageEvents.h"
#include "Navigation/CrowdFollowingComponent.h"
#include "AI/Squads/BasicSquad.h"

ABasicAIController::ABasicAIController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UCrowdFollowingComponent>(TEXT("PathFollowingComponent")))
{
	BehaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("Behavior Tree Component"));
	BlackboardComponent = CreateDefaultSubobject<UBlackboardComponent>(TEXT("Blackboard Component"));
	bCanBeSelect = true;

	// Character Statue Initialization
	CanAttack = true;
	VisionRadius = 0.05f;
	AttackRange = 50.0f;
	WeaponDamage = 1.0f;
	AttackInterval = 1.0f;
	Health = 10.0;
	DamageTypeClass = UDamageType_RegularBullet::StaticClass();

	AIPresecptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("Perception Component"));
}

void ABasicAIController::BeginPlay()
{
	Super::BeginPlay();
	// initialize the behavior tree
	if (IsValid(BehaviorTree.Get()))
	{
		RunBehaviorTree(BehaviorTree.Get());
		BehaviorTreeComponent->StartTree(*BehaviorTree.Get());
	}

	// Initialize the weapon timer
	// TODO::Move this into a weapon class
	GetWorldTimerManager().SetTimer(WeaponCountdownTimerHandle, this, &ABasicAIController::PerformAttackTarget, AttackInterval, true);
	GetWorldTimerManager().PauseTimer(WeaponCountdownTimerHandle);

	TObjectPtr<ABasicCharacter> character = Cast<ABasicCharacter>(GetPawn());
	if (character)
		character->UpdateRevealRadius();
}

void ABasicAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (IsValid(Blackboard.Get()) && IsValid(BehaviorTree))
	{
		Blackboard->InitializeBlackboard(*BehaviorTree.Get()->BlackboardAsset.Get());
	}
}

void ABasicAIController::OnMoveCompleted(FAIRequestID RequestID, EPathFollowingResult::Type Result)
{
	TObjectPtr<UMoveCommand> command = Cast<UMoveCommand>(BlackboardComponent->GetValueAsObject(UBTTask_BasicTask::COMMAND_BLACKBOARD_NAME));
	if (!command)
		return;
	EBTNodeResult::Type TaskResult{EBTNodeResult::Failed};
	switch (Result)
	{
	case EPathFollowingResult::Success:
		TaskResult = EBTNodeResult::Succeeded;
		break;
	case EPathFollowingResult::Blocked:
		TaskResult = EBTNodeResult::Failed;
		break;
	case EPathFollowingResult::OffPath:
		TaskResult = EBTNodeResult::Failed;
		break;
	case EPathFollowingResult::Aborted:
		command->AbortCommand();
		return;
	case EPathFollowingResult::Invalid:
		TaskResult = EBTNodeResult::Failed;
		break;
	default:
		break;
	}
	command->StopCommand(TaskResult);
}

void ABasicAIController::Dead(TWeakObjectPtr<AActor> Causer)
{
	TObjectPtr<ABasicCharacter> CharacterPawn = GetPawn<ABasicCharacter>();
	if (!CharacterPawn)
		return;

	GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Yellow, FString::Printf(TEXT("%s dead!"), *CharacterPawn->GetName()));
	
	// Inform all the attackers the target is dead
	auto& attackersCopy = AttackersController;
	for (auto& attacker : attackersCopy)
	{
		if (attacker.GetObject())
			IAttackableInterface::Execute_AttackTargetKilled(attacker.GetObject(), CharacterPawn);
		else
			AttackersController.Remove(attacker);
	}

	TWeakObjectPtr<ABasicSquad> squad = CharacterPawn->GetSquad();
	squad->RemoveSquadMember(CharacterPawn);

	CharacterPawn->Destroy();
}

bool ABasicAIController::SendMoveToLocationCommand(FVector Location)
{
	TObjectPtr<UMoveCommand> command = NewObject<UMoveCommand>(this, UMoveCommand::StaticClass());
	command->SetTargetLocation(Location);
	command->commandTypes = ECommandTypes::TargetLocationCommand;
	command->OwnerBehaviorTree = BehaviorTreeComponent;
	BlackboardComponent->SetValueAsObject(FName(TEXT("NewCommand")), command.Get());
	return true;
}


float ABasicAIController::GetVisionRadius() const
{
	return VisionRadius;
}

void ABasicAIController::SetVisionRadius(float newRadius)
{
	VisionRadius = newRadius;
	TObjectPtr<ABasicCharacter> character = Cast<ABasicCharacter>(GetPawn());
	if (character)
		character->UpdateRevealRadius();
}

void ABasicAIController::StopAndClearAllCommand()
{
	TWeakObjectPtr<UBasicCommand> command = Cast<UBasicCommand>(BlackboardComponent->GetValueAsObject(UBTTask_BasicTask::COMMAND_BLACKBOARD_NAME));
	if (!command.IsValid() || !command->Task.IsValid())
		return;

	command->Task->StopTask(BehaviorTreeComponent, EBTNodeResult::Aborted);
	command->StopCommand(EBTNodeResult::Succeeded);
	command->MarkAsGarbage();
	BlackboardComponent->ClearValue(UBTTask_BasicTask::COMMAND_BLACKBOARD_NAME);
}

bool ABasicAIController::ConfirmAndAttackTarget(const TScriptInterface<IAttackableInterface>& TargetController)
{
	TObjectPtr<ABasicCharacter> CharacterPawn = GetPawn<ABasicCharacter>();
	if (!CharacterPawn)
		return false;

	if (!TargetController || !CanAttack || !IAttackableInterface::Execute_CanBeAttacked(TargetController.GetObject(), CharacterPawn, DamageTypeClass))
		return false;

	SetAttackTarget(TargetController);
	return true;
}

void ABasicAIController::SetAttackTarget(const TScriptInterface<IAttackableInterface>& Target)
{
	ClearAttackTarget();

	TObjectPtr<ABasicCharacter> CharacterPawn = GetPawn<ABasicCharacter>();
	if (!CharacterPawn)
		return;

	CharacterPawn->IsAttacking = true;
	BlackboardComponent->SetValueAsObject(UBTTask_Attack::TARGET_BLACKBOARD_NAME, Target.GetObject());
}

void ABasicAIController::ClearAttackTarget()
{
	TObjectPtr<ABasicCharacter> CharacterPawn = GetPawn<ABasicCharacter>();
	if (!CharacterPawn)
		return;

	CharacterPawn->IsAttacking = false;
	BlackboardComponent->ClearValue(UBTTask_Attack::TARGET_BLACKBOARD_NAME);
	OnAttackTaskFinished.ExecuteIfBound(BehaviorTreeComponent, EBTNodeResult::Succeeded);
}

void ABasicAIController::AttackTargetKilled(const TScriptInterface<IAttackableInterface>& DyingTarget)
{
	TObjectPtr<ABasicCharacter> CharacterPawn = GetPawn<ABasicCharacter>();
	if (!CharacterPawn)
		return;

	TScriptInterface<IAttackableInterface> AttackTargetInterface = BlackboardComponent->GetValueAsObject(UBTTask_Attack::TARGET_BLACKBOARD_NAME);
	if (!AttackTargetInterface || DyingTarget.GetObject() != AttackTargetInterface.GetObject())
		return;

	ClearAttackTarget();
	// TODO:: Switch target function here

	TWeakObjectPtr<ABasicCharacter> DyingCharcter = Cast<ABasicCharacter>(DyingTarget.GetObject());
	if (!DyingCharcter.IsValid())
		return;

	auto Squad = DyingCharcter->GetSquad();
	auto SquadMembers = Squad->GetSquadMemebers();
	SquadMembers.Remove(DyingCharcter.Get());

	if (!SquadMembers.IsEmpty())
	{
		auto nextTarget = SquadMembers[FMath::RandRange(0, SquadMembers.Num() - 1)];
		IAttackableInterface::Execute_ConfirmAndAttackTarget(CharacterPawn, nextTarget);
	}
}

void ABasicAIController::PerformAttackTarget()
{
	TObjectPtr<ABasicCharacter> CharacterPawn = GetPawn<ABasicCharacter>();
	if (!CharacterPawn)
		return;

	auto TargetUnderAttack = BlackboardComponent->GetValueAsObject(UBTTask_Attack::TARGET_BLACKBOARD_NAME);
	if (!TargetUnderAttack)
	{
		IAttackableInterface::Execute_DisableWeapons(CharacterPawn);
		ClearAttackTarget();
		return;
	}

	IAttackableInterface::Execute_TakeDamage(TargetUnderAttack, WeaponDamage, FDamageEvent(), this, CharacterPawn);
}

bool ABasicAIController::CanBeAttacked(const TScriptInterface<IAttackableInterface>& Attacker, TSubclassOf<UDamageType> DamageType)
{
	return true;
}

void ABasicAIController::AttackFeedBack(EAttackResult Result, const TScriptInterface<IAttackableInterface>& Target)
{
	
}

void ABasicAIController::OnReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	TObjectPtr<ABasicCharacter> CharacterPawn = Cast<ABasicCharacter>(GetPawn());
	if (!CharacterPawn)
		return;

	Health -= Damage;

	GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Yellow, FString::Printf(TEXT("%s hit %s %f hp!"), *DamageCauser->GetName(), *GetName(), &Damage));

	if (Health <= 0)
	{
		IAttackableInterface::Execute_AttackFeedBack(DamageCauser, EAttackResult::DEAD, this);

		Dead(DamageCauser);
		return;
	}

	IAttackableInterface::Execute_AttackFeedBack(DamageCauser, EAttackResult::HIT, this);
}

TScriptInterface<IAttackableInterface> ABasicAIController::GetAttackingTarget() const
{
	return BlackboardComponent->GetValueAsObject(UBTTask_Attack::TARGET_BLACKBOARD_NAME);;
}

void ABasicAIController::EnableWeapons()
{
	if (!GetWorldTimerManager().IsTimerActive(WeaponCountdownTimerHandle))
		GetWorldTimerManager().UnPauseTimer(WeaponCountdownTimerHandle);
}

void ABasicAIController::DisableWeapons()
{
	if (GetWorldTimerManager().IsTimerActive(WeaponCountdownTimerHandle))
		GetWorldTimerManager().PauseTimer(WeaponCountdownTimerHandle);
}


void ABasicAIController::SetAsAttacker(const TScriptInterface<IAttackableInterface>& Attacker)
{
	if (!Attacker)
		return;
	
	AttackersController.AddUnique(Attacker);
}

void ABasicAIController::RemoveFromAttackers(const TScriptInterface<IAttackableInterface>& Attacker)
{
	if (!Attacker)
	{
		AttackersController.Remove(nullptr);
		return;
	}

	AttackersController.Remove(Attacker);
}
