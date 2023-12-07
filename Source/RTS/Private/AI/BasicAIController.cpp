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

	OnTakeAnyDamage.AddDynamic(this, &ABasicAIController::ReceiveDamage);
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
	TObjectPtr<ABasicCharacter> CharacterPawn = Cast<ABasicCharacter>(GetPawn());
	if (!CharacterPawn)
		return;

	GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Yellow, FString::Printf(TEXT("%s dead!"), *CharacterPawn->GetName()));
	
	// Inform all the attackers the target is dead
	auto attackersCopy = AttackersController;
	for (auto attacker : attackersCopy)
	{
		if (attacker.GetObject())
			IAttackableInterface::Execute_AttackTargetKilled(attacker.GetObject(), this);
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

bool ABasicAIController::ConfirmAndAttackTarget_Implementation(const TScriptInterface<IAttackableInterface>& TargetController)
{
	if (!TargetController || !CanAttack || !IAttackableInterface::Execute_CanBeAttacked(TargetController.GetObject(), this, DamageTypeClass))
		return false;

	SetAttackTarget(TargetController);
	return true;
}

void ABasicAIController::SetAttackTarget(const TScriptInterface<IAttackableInterface>& Target)
{
	ClearAttackTarget();

	TObjectPtr<ABasicCharacter> CharacterPawn = Cast<ABasicCharacter>(GetPawn());
	if (!CharacterPawn)
		return;

	CharacterPawn->IsAttacking = true;
	BlackboardComponent->SetValueAsObject(UBTTask_Attack::TARGET_BLACKBOARD_NAME, Target.GetObject());
}

void ABasicAIController::ClearAttackTarget()
{
	TObjectPtr<ABasicCharacter> CharacterPawn = Cast<ABasicCharacter>(GetPawn());
	if (!CharacterPawn)
		return;

	CharacterPawn->IsAttacking = false;
	BlackboardComponent->ClearValue(UBTTask_Attack::TARGET_BLACKBOARD_NAME);
	OnAttackTaskFinished.ExecuteIfBound(BehaviorTreeComponent, EBTNodeResult::Succeeded);
}

void ABasicAIController::AttackTargetKilled_Implementation(const TScriptInterface<IAttackableInterface>& DyingTargetController)
{
	TScriptInterface<IAttackableInterface> AttackTargetInterface = BlackboardComponent->GetValueAsObject(UBTTask_Attack::TARGET_BLACKBOARD_NAME);
	if (!AttackTargetInterface || DyingTargetController.GetObject() != AttackTargetInterface.GetObject())
		return;

	ClearAttackTarget();
	// TODO:: Switch target function here
}

void ABasicAIController::PerformAttackTarget()
{
	TObjectPtr<ABasicCharacter> CharacterPawn = Cast<ABasicCharacter>(GetPawn());
	if (!CharacterPawn)
		return;

	auto AttackTargetController = Cast<AController>(BlackboardComponent->GetValueAsObject(UBTTask_Attack::TARGET_BLACKBOARD_NAME));
	if (AttackTargetController == nullptr)
	{
		IAttackableInterface::Execute_DisableWeapons(this);
		return;
	}

	TWeakObjectPtr<APawn> AttackTarget = Cast<APawn>(AttackTargetController->GetPawn());
	if (!AttackTarget.IsValid() || !AttackTargetController->Implements<UAttackableInterface>())
	{
		ClearAttackTarget();
		return;
	}

	AttackTargetController->TakeDamage(WeaponDamage, FDamageEvent(), this, CharacterPawn);
}

bool ABasicAIController::CanBeAttacked_Implementation(const TScriptInterface<IAttackableInterface>& AttackerController, TSubclassOf<UDamageType> DamageType)
{
	return true;
}

void ABasicAIController::AttackFeedBack_Implementation(EAttackResult Result, const TScriptInterface<IAttackableInterface>& TargetController)
{
	
}

void ABasicAIController::ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	Health -= Damage;

	GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Yellow, FString::Printf(TEXT("%s hit %s %f hp!"), *DamageCauser->GetName(), *GetName(), &Damage));

	if (Health <= 0)
	{
		IAttackableInterface::Execute_AttackFeedBack(InstigatedBy, EAttackResult::DEAD, this);

		Dead(DamageCauser);
		return;
	}

	IAttackableInterface::Execute_AttackFeedBack(InstigatedBy, EAttackResult::HIT, this);
}

void ABasicAIController::EnableWeapons_Implementation()
{
	if (!GetWorldTimerManager().IsTimerActive(WeaponCountdownTimerHandle))
		GetWorldTimerManager().UnPauseTimer(WeaponCountdownTimerHandle);
}

void ABasicAIController::DisableWeapons_Implementation()
{
	if (GetWorldTimerManager().IsTimerActive(WeaponCountdownTimerHandle))
		GetWorldTimerManager().PauseTimer(WeaponCountdownTimerHandle);
}

void ABasicAIController::SetAsAttacker_Implementation(const TScriptInterface<IAttackableInterface>& AttackerController)
{
	if (!AttackerController.GetObject())
		return;
	
	AttackersController.AddUnique(AttackerController);
}

void ABasicAIController::RemoveFromAttackers_Implementation(const TScriptInterface<IAttackableInterface>& AttackerController)
{
	AttackersController.Remove(AttackerController);
}
