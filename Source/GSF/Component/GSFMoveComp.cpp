
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
	MaxWalkSpeed = moveVelocity;
	GroundFriction = friction;
	GravityScale = gravity;
	JumpZVelocity = jumpPower;
}

void UGSFMoveComp::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	MovementMode = MOVE_Falling;

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
	// 親クラスのPhysWalkingを呼び出す
	Super::PhysWalking(deltaTime, Iterations);
}

void UGSFMoveComp::RegisterMethods(UMethodExecutor* Executor)
{
	Executor->AddMethod([this](){Jump();}, EAction::Jump);
	Executor->AddMethod([this](){HighJump();}, EAction::HighJump);
	Executor->AddMethod([this](){Dodge();}, EAction::Dodge);
	Executor->AddMethod([this](){LongDodge();}, EAction::LongDodge);
	Executor->AddMethod([this](){Fly();}, EAction::Fly);
	Executor->AddMethod([this](){Glide();}, EAction::Glide);

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

		AddInputVector(moveVector * moveVelocity * Value, false);
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

		AddInputVector(moveVector * moveVelocity * Value, false);
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
	launchVector.Z = bigJumpPower;
	
	Launch(launchVector);
}

void UGSFMoveComp::Dodge()
{
	if(!IsValid(character))return;
	if(!IsValid(character->cameraComp))return;
	
	// 垂直ジャンプ
	FVector launchVector = character->cameraComp->camera->GetForwardVector() * input.Y + character->cameraComp->camera->GetRightVector() * input.X;
	launchVector *= dodgePower;
	Launch(launchVector);
}

void UGSFMoveComp::LongDodge()
{
	if(!IsValid(character))return;
	if(!IsValid(character->cameraComp))return;
	
	// 垂直ジャンプ
	FVector launchVector = character->cameraComp->camera->GetForwardVector() * input.Y + character->cameraComp->camera->GetRightVector() * input.X;
	launchVector *= bigDodgePower;
	Launch(launchVector);
}

void UGSFMoveComp::Fly()
{
	
}

void UGSFMoveComp::Glide()
{
	
}

void UGSFMoveComp::Landing()
{
	// どうしよう
	if(!IsValid(character))return;
}


