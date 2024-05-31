// Fill out your copyright notice in the Description page of Project Settings.


#include "GSFCharacter.h"

#include "GSF/Component/GSFAbilityComp.h"
#include "GSF/Component/GSFInputComp.h"
#include "GSF/Component/GSFMoveComp.h"
#include "GSF/Component/GSFAnimComp.h"
#include "GSF/Camera/GSFCamera.h"

#include "Components/CapsuleComponent.h"

AGSFCharacter::AGSFCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UGSFMoveComp>(ACharacter::CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = true;

	executor = CreateDefaultSubobject<UMethodExecutor>(TEXT("Executor"));
	moveComp = Cast<UGSFMoveComp>(GetCharacterMovement());
	inputComp = CreateDefaultSubobject<UGSFInputComp>(TEXT("Input"));
	animComp = CreateDefaultSubobject<UGSFAnimComp>(TEXT("Anim"));
	abilityComp = CreateDefaultSubobject<UGSFAbilityComp>(TEXT("Ability"));

	if(moveComp)
	{
		moveComp->RegisterMethods(executor);
	}
	if(abilityComp)
	{
		abilityComp->RegisterMethods(executor);
	}
	
	if(GetMesh())
	{
		GetMesh()->SetRelativeRotation(FRotator(0, -90.f,0));
		GetMesh()->SetRelativeLocation(FVector(0,0,-50.f));
	}

	if(GetCapsuleComponent())
	{
		GetCapsuleComponent()->InitCapsuleSize(26.f, 52.f);
	}

	AutoPossessPlayer = EAutoReceiveInput::Player0;
}

void AGSFCharacter::BeginPlay()
{
	Super::BeginPlay();

	// 入力ハンドラー設定
	if(IsValid(inputComp))
	{
		inputComp->onInputEvent.BindDynamic(this, &AGSFCharacter::InputHandler);
		inputComp->onAxisInputEvent.BindDynamic(this, &AGSFCharacter::AxisInputHandler);
	}

	// HP設定
	health = maxHealth;
}

void AGSFCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AGSFCharacter::RestartPlayer()
{
	health = maxHealth;
}

void AGSFCharacter::SubtractHealth(const FAbilityData& Data)
{
	// HP計算
	health = FMath::Clamp(health - Data.Damage, 0.f, maxHealth);
	
	if(health == 0.f)
	{
		// 死亡
		executor->ExecuteActionMethod(EAction::Death);
	}
	else
	{
		// 怯み
		executor->ExecuteActionMethod(EAction::Fear, Data.FearTime);
	}
	// 吹き飛び
	if(IsValid(GetMoveComp()))
	{
		GetMoveComp()->AddForce(Data.ForceVector * Data.Force);
	}
}

bool AGSFCharacter::ReceiveDamage(const FAbilityData& Data)
{
	if (Data.Damage < 0)return false;

	// hp計算
	SubtractHealth(Data);

	return true;
}

void AGSFCharacter::ResetCharacterInputPermission()
{
	SetTakeAction(false);
	SetCantMove(false);
	SetCantBufferedInput(false);
}

void AGSFCharacter::InputHandler(EInput Input, EInputType Type)
{
	if(bCantInput)return;
	
	switch (Input)
	{
	case EInput::Forward:
		break;
	case EInput::Right:
		break;
	case EInput::Pitch:
		break;
	case EInput::Yaw:
		break;
	case EInput::L1:
		if(Type == EInputType::Long)
		{
			executor->ExecuteMethod(EAction::LockOn);
		}
		else if(Type == EInputType::Release)
		{

		}
		break;
	case EInput::L2:
		if(Type == EInputType::Long)
		{
			executor->ExecuteMethod(EAction::Concentration);
		}
		else if(Type == EInputType::Release)
		{

		}
		break;
	case EInput::R1:
		if(Type == EInputType::Single)
		{
			executor->ExecuteMethod(EAction::Dodge);
		}
		else if(Type == EInputType::Double)
		{
			executor->ExecuteMethod(EAction::Fly);
		}
		else if(Type == EInputType::Long)
		{
			executor->ExecuteMethod(EAction::Glide);
		}
		else if(Type == EInputType::Release)
		{

		}
		break;
	case EInput::R2:
		if(Type == EInputType::Long)
		{
			executor->ExecuteActionMethod(EAction::Shot);
		}
		else if(Type == EInputType::Release)
		{

		}
		break;
	case EInput::A:
		break;
	case EInput::B:
		if(Type == EInputType::Single)
		{
			executor->ExecuteActionMethod(EAction::Attack);
		}
		if(Type == EInputType::Long)
		{
			executor->ExecuteActionMethod(EAction::HeavyAttack);
		}
		break;
	case EInput::X:
		break;
	case EInput::Y:
		break;
	default:
		break;
	}
}

void AGSFCharacter::AxisInputHandler(EInput Input, EInputType Type, float Value)
{
	if(bCantInput)return;
	
	switch (Input)
	{
	case EInput::Forward:
		if(bCantMove)return;
		executor->ExecuteAxisMethod(EAction::MoveForward, Value);
		break;
	case EInput::Right:
		if(bCantMove)return;
		executor->ExecuteAxisMethod(EAction::MoveRight, Value);
		break;
	case EInput::Pitch:
		if(bCantCamera)return;
		if(IsValid(cameraComp)) cameraComp->InputAxis_Pitch(Value);
		break;
	case EInput::Yaw:
		if(bCantCamera)return;
		if(IsValid(cameraComp)) cameraComp->InputAxis_Yaw(Value);
		break;
	default:
		break;
	}
}