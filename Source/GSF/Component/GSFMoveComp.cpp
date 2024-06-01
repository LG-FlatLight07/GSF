
#include "GSFMoveComp.h"
#include "Components/CapsuleComponent.h"

#include "GSF/Character/GSFCharacter.h"
#include "GSFAbilityComp.h"
#include "GSF/Camera/GSFCamera.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

void UGSFMoveComp::BeginPlay()
{
	Super::BeginPlay();

	character = Cast<AGSFCharacter>(GetOwner());

	//-------------------------------------------------
	MaxWalkSpeed = moveSpeed;
	MaxFlySpeed = moveSpeed;
	GroundFriction = friction;
	GravityScale = gravity;
	JumpZVelocity = jumpPower;
	AirControl = control;
}

void UGSFMoveComp::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	SetMovementMode(MOVE_Falling);
	UpdateIsInAir();
}

void UGSFMoveComp::AddInputVector(FVector WorldVector, bool bForce)
{
	Super::AddInputVector(WorldVector, bForce);
}

void UGSFMoveComp::AddForce(FVector Force)
{
	if (!Force.IsZero() && IsActive() && HasValidData())
	{
		Launch(Force);
	}
}

FVector UGSFMoveComp::ConstrainAnimRootMotionVelocity(const FVector& RootMotionVelocity, const FVector& CurrentVelocity) const
{
	// RootMotionに移動入力を反映
	return RootMotionVelocity + CurrentVelocity * correctRootVelocityBiasByInput;
}

bool UGSFMoveComp::IsFalling() const
{
	return Super::IsFalling();
}

void UGSFMoveComp::PhysWalking(float deltaTime, int32 Iterations)
{
	Super::PhysWalking(deltaTime, Iterations);
}

void UGSFMoveComp::PhysFalling(float deltaTime, int32 Iterations)
{
	Super::PhysFalling(deltaTime, Iterations);
}

void UGSFMoveComp::PhysFlying(float deltaTime, int32 Iterations)
{
	Super::PhysFlying(deltaTime, Iterations);
}

void UGSFMoveComp::RegisterMethods(UMethodExecutor* Executor)
{
	Executor->AddMethod([this](){Jump();}, EAction::Jump);
	Executor->AddMethod([this](){HighJump();}, EAction::HighJump);
	Executor->AddMethod([this](){Dodge();}, EAction::Dodge);
	Executor->AddMethod([this](){LongDodge();}, EAction::LongDodge);
	Executor->AddMethod([this](){Fly();}, EAction::Fly);
	Executor->AddMethod([this](){Glide();}, EAction::Glide);
	Executor->AddMethod([this](){Glide_End();}, EAction::GlideEnd);

	Executor->AddAxisMethod([this](float Value){MoveRight(Value);}, EAction::MoveRight);
	Executor->AddAxisMethod([this](float Value){MoveForward(Value);}, EAction::MoveForward);
}

void UGSFMoveComp::UpdateIsInAir()
{
	if(!IsValid(character))return;
	
	// 接地検証レイ
	FVector startPos = character->GetActorLocation();
	startPos.Z -= character->GetCapsuleHalfHeight();
	FVector endPos = startPos;
	endPos.Z -= rayCastLengthFromAir;

	// 自身は無視 << 相手プレイヤーも無視
	FCollisionQueryParams params;
	params.AddIgnoredActor(character);

	// 足元チェック
	FHitResult outHit;
	GetWorld()->LineTraceSingleByChannel(outHit, startPos, endPos, ECC_WorldStatic, params);
	if (outHit.bBlockingHit)
	{
		// 着地
		if(bAir)
		{
			Landing();
		}
		bAir = false;
	}
	else
	{
		bAir = true;
	}

	// キャラ反映
	character->SetIsInAir(bAir);
}

void UGSFMoveComp::MoveForward(float Value)
{
	input.Y = Value;

	if(IsValid(character->cameraComp))
	{
		// 移動
		FVector moveVector = character->cameraComp->camera->GetForwardVector();
		moveVector.Z = 0.f;

		character->AddMovementInput(moveVector, moveSpeed * Value);
		//AddInputVector(moveVector * moveVelocity * Value, false);
	}
}

void UGSFMoveComp::MoveRight(float Value)
{
	input.X = Value;
	
	if(IsValid(character->cameraComp))
	{
		// 移動
		FVector moveVector = character->cameraComp->camera->GetRightVector();
		moveVector.Z = 0.f;

		character->AddMovementInput(moveVector, moveSpeed * Value);
		//AddInputVector(moveVector * moveVelocity * Value, false);
	}
}

void UGSFMoveComp::Jump()
{
	if(!IsValid(character))return;
	
	// 垂直ジャンプ
	FVector launchVector = FVector::ZeroVector;
	launchVector.Z = jumpPower;
	
	Launch(launchVector);
}

void UGSFMoveComp::HighJump()
{
	if(!IsValid(character))return;
	
	// 垂直ジャンプ
	FVector launchVector = FVector::ZeroVector;
	launchVector.Z = highJumpPower;
	
	Launch(launchVector);
}

void UGSFMoveComp::Dodge()
{
	if(!IsValid(character))return;
	if(!IsValid(character->cameraComp))return;
	
	FVector forward = character->cameraComp->camera->GetForwardVector();
	forward.Z = 0.f;
	FVector right = character->cameraComp->camera->GetRightVector();
	right.Z = 0.f;
	FVector launch = forward * input.Y + right * input.X;
	launch *= dodgePower;
	Launch(launch);
}

void UGSFMoveComp::LongDodge()
{
	if(!IsValid(character))return;
	if(!IsValid(character->cameraComp))return;
	
	FVector forward = character->cameraComp->camera->GetForwardVector();
	forward.Z = 0.f;
	FVector right = character->cameraComp->camera->GetRightVector();
	right.Z = 0.f;
	FVector launch = forward * input.Y + right * input.X;
	launch *= longDodgePower;
	Launch(launch);
}

void UGSFMoveComp::Fly()
{
	
}

void UGSFMoveComp::Glide()
{
	MaxFlySpeed = glideSpeed;
	GravityScale = glideGravity;
	AirControl = glideControl;
}

void UGSFMoveComp::Glide_End()
{
	MaxFlySpeed = moveSpeed;
	GravityScale = gravity;
	AirControl = control;
}

void UGSFMoveComp::Landing()
{
	// どうしよう
	if(!IsValid(character))return;
}


