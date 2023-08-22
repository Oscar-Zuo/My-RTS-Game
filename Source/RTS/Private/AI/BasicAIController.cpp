// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BasicAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AI/Commands/MoveCommand.h"
#include "Pawn/BasicCharacter.h"
#include "AI/Tasks/BTTask_BasicTask.h"
#include "AI/Tasks/BTTask_Attack.h"
#include "Navigation/CrowdFollowingComponent.h"

ABasicAIController::ABasicAIController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UCrowdFollowingComponent>(TEXT("PathFollowingComponent")))
{
	BehaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("Behavior Tree Component"));
	BlackboardComponent = CreateDefaultSubobject<UBlackboardComponent>(TEXT("Blackboard Component"));
	bCanBeSelect = true;

	// Character Statue Initialization
	CanAttack = true;
	AttackRange = 50.0f;
	AttackDamage = 1.0f;
	AttackInterval = 1000.0f;
	Health = 10.0;
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
	DisableWeapon();
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

bool ABasicAIController::SendMoveToLocationCommand(FVector Location)
{
	TObjectPtr<UMoveCommand> command = NewObject<UMoveCommand>(this, UMoveCommand::StaticClass());
	command->SetTargetLocation(Location);
	command->commandTypes = ECommandTypes::TargetLocationCommand;
	command->OwnerBehaviorTree = BehaviorTreeComponent;
	BlackboardComponent->SetValueAsObject(FName(TEXT("NewCommand")), command.Get());
	return true;
}

void ABasicAIController::StopAndClearAllCommand()
{
	TWeakObjectPtr<UBasicCommand> command = Cast<UBasicCommand>(BlackboardComponent->GetValueAsObject(UBTTask_BasicTask::COMMAND_BLACKBOARD_NAME));
	if (!command.IsValid())
		return;

	command->StopCommand(EBTNodeResult::Succeeded);
	command->MarkAsGarbage();
	BlackboardComponent->ClearValue(UBTTask_BasicTask::COMMAND_BLACKBOARD_NAME);
}

bool ABasicAIController::ConfirmAndAttackTarget(const TScriptInterface<IAttackableInterface>& Target)
{
	if (!CanAttack || !IAttackableInterface::Execute_CanBeAttacked(Target.GetObject(), this))
		return false;
	SetAttackTarget(Target);

	return true;
}

void ABasicAIController::SetAttackTarget(const TScriptInterface<IAttackableInterface>& Target)
{
	TObjectPtr<ABasicCharacter> CharacterPawn = Cast<ABasicCharacter>(GetPawn());
	if (!CharacterPawn)
		return;

	BlackboardComponent->SetValueAsObject(UBTTask_Attack::TARGET_BLACKBOARD_NAME, Target.GetObject());
}

void ABasicAIController::ClearAttackTarget()
{
	TObjectPtr<ABasicCharacter> CharacterPawn = Cast<ABasicCharacter>(GetPawn());
	if (!CharacterPawn)
		return;

	CharacterPawn->IsAttacking = true;
	BlackboardComponent->SetValueAsObject(UBTTask_Attack::TARGET_BLACKBOARD_NAME, NULL);
}

void ABasicAIController::PerformAttackTarget()
{
	TObjectPtr<ABasicCharacter> CharacterPawn = Cast<ABasicCharacter>(GetPawn());
	if (!CharacterPawn)
		return;

	CharacterPawn->IsAttacking = true;

	TScriptInterface<IAttackableInterface> AttackTarget = BlackboardComponent->GetValueAsObject(UBTTask_Attack::TARGET_BLACKBOARD_NAME);

	auto AttackResult = IAttackableInterface::Execute_ReceiveDamage(AttackTarget.GetObject(), CharacterPawn, AttackDamage);
	
	if (AttackResult == EAttackResult::FAILED || EAttackResult::DEAD)
		DisableWeapon();
}

bool ABasicAIController::CanBeAttacked_Implementation(TWeakObjectPtr<AActor> Attacker)
{
	return true;
}

TEnumAsByte<EAttackResult> ABasicAIController::ReceiveDamage_Implementation(TWeakObjectPtr<AActor> Attacker, float Damage)
{
	Health -= Damage;
	if (Health <= 0)
	{
		// TODO:: replace this with actual death function
		TObjectPtr<ABasicCharacter> CharacterPawn = Cast<ABasicCharacter>(GetPawn());
		if (!CharacterPawn)
			return EAttackResult::FAILED;
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Yellow, FString::Printf(TEXT("%s dead!"), *CharacterPawn.GetName()));
		CharacterPawn->Destroy();
		Destroy();
		return EAttackResult::DEAD;
	}

	return EAttackResult::HIT;
}

void ABasicAIController::EnableWeapon()
{
	if (!GetWorldTimerManager().IsTimerActive(WeaponCountdownTimerHandle))
		GetWorldTimerManager().UnPauseTimer(WeaponCountdownTimerHandle);
}

void ABasicAIController::DisableWeapon()
{
	if (GetWorldTimerManager().IsTimerActive(WeaponCountdownTimerHandle))
		GetWorldTimerManager().PauseTimer(WeaponCountdownTimerHandle);
}
