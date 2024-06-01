#include "GSFMoveComp.h"

#include "GSF/Character/GSFCharacter.h"
#include "GSF/Component/GSFAbilityComp.h"
#include "Components/ArrowComponent.h"
#include "Runtime/Engine/Classes/Animation/AnimInstance.h"

#include "Kismet/KismetMathLibrary.h"
#include "DrawDebugHelpers.h"
#include "GSF/GameMode/GSFGameMode.h"
#include "Kismet/GameplayStatics.h"

#define TIME UGameplayStatics::GetGlobalTimeDilation(GetWorld())

UGSFMoveComp::UGSFMoveComp()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UGSFMoveComp::BeginPlay()
{
	Super::BeginPlay();

	character = Cast<AGSFCharacter>(GetOwner());
}

/// @brief 入力
/// @param InputValue 
void UGSFMoveComp::AddForwardMovementInput(float InputValue)
{
	if (InputValue == 0.f)
	{
		bInputFwd = false;
		if (inputBias.Y >= 0)
			inputBias.Y = FMath::Clamp(inputBias.Y - stopPower, 0, inputBias.Y);
		else
			inputBias.Y = FMath::Clamp(inputBias.Y + stopPower, inputBias.Y, 0);
	}
	else
	{
		bInputFwd = true;
		inputBias.Y = FMath::Clamp(inputBias.Y + InputValue * airControll, -1, 1); //
	}
	inputValue.Y = InputValue;
}

/// @brief 入力
/// @param InputValue 
void UGSFMoveComp::AddRightMovementInput(float InputValue)
{
	// 入力が無ければ０に補正していく
	if (InputValue == 0.f)
	{
		bInputRight = false;
		if (inputBias.X >= 0)
			inputBias.X = FMath::Clamp(inputBias.X - stopPower, 0, inputBias.X);
		else
			inputBias.X = FMath::Clamp(inputBias.X + stopPower, inputBias.X, 0);
	}
	else
	{
		bInputRight = true;
		inputBias.X = FMath::Clamp(inputBias.X + InputValue * airControll, -1, 1); //
	}
	inputValue.X = InputValue;
}

void UGSFMoveComp::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ルートモーション
	UpdateRootMosion(DeltaTime);
	// 移動
	UpdateMove(DeltaTime);
	// 吹っ飛び
	UpdateForce(DeltaTime);
	// 飛翔
	UpdateFly(DeltaTime);
	// 重力
	UpdateGravity(DeltaTime);
	// 回転
	UpdateRotation(DeltaTime);

	// Wire移動
	// いつか書き直します
	// if (bWire)
	// {
	// 	float frame;
	// 	frame = 1.0f / DeltaTime;
	// 	FVector_NetQuantizeNormal outHitNormal;
	// 	if (SweepMove(wireMoveVector, (wireMoveSpeed / frame) * TIME/*一時的な奴です*/, &outHitNormal))
	// 	{
	// 		FVector alongWallVector = GetAlongWallVector(outHitNormal, wireMoveVector);
	// 		SweepMove(alongWallVector, wireMoveSpeed / frame * TIME/*一時的な奴です*/, &outHitNormal);
	// 	}
	// 	currentWireMoveTime += DeltaTime * TIME/*一時的な奴です*/;
	// 	if (wireMoveTime <= currentWireMoveTime)
	// 	{
	// 		bWire = false;
	// 		currentWireMoveTime = 0;
	// 	}
	// }

	// 進行ベクトルを保存
	if(!inputValue.IsZero())
		beforeMoveVector = inputValue;
	beforePos = character->GetActorLocation();
}

/// @brief 力を加える
/// @param Vector 方向ベクトル
/// @param Value 吹っ飛び力
/// @param Deceleration 減速力
void UGSFMoveComp::AddForce(FVector Vector, float Value, float Deceleration)
{
	Vector.Normalize();
	forceVector = Vector;
	forceValue = Value;
	forceDecelerationValue = Deceleration;
	bForce = true;

	// 吹き飛び以外の挙動をキャンセル
	bFly = false;
	bWire = false;
	bDive = false;
	beforeGravityAcceleration = 0;
	currentWireMoveTime = 0;
	ResetAirbornTime();
	ResetGravityBias();
	ResetSpeedBias();
}

void UGSFMoveComp::AirDash()
{
	AGSFCamera* camera = Cast<AGSFGameMode>(UGameplayStatics::GetGameMode(GetWorld()))->camera;
	if(IsValid(camera))
	{
		// カメラベクトル
		FVector Fwd = FVector(camera->arrow->GetForwardVector().X, camera->arrow->GetForwardVector().Y, 0);
		FVector Right = FVector(camera->arrow->GetRightVector().X, camera->arrow->GetRightVector().Y, 0);
		// 移動方向
		FVector moveVector = Fwd * inputValue.Y + Right * inputValue.X;
		moveVector.Z += 0.1f;
		AddForce(moveVector, airDashSpeed, airDashDeceleration);

		camera->ResetBoomLength();
		camera->ResetPitch();
	}
}

void UGSFMoveComp::AirJump()
{
	AGSFCamera* camera = Cast<AGSFGameMode>(UGameplayStatics::GetGameMode(GetWorld()))->camera;
	if(IsValid(camera))
	{
		// 移動方向
		AddForce(FVector(0,0, 1), airJumpPower, airJumpDeceleration);
		camera->ResetBoomLength();
		camera->ResetPitch();
	}
}

/// @brief 飛翔
void UGSFMoveComp::Fly()
{
	flyDurationTimer = 0;
	currentFlyVelocityValue = flyVelocityValue;
	bFly = true;

	AGSFCamera* camera = Cast<AGSFGameMode>(UGameplayStatics::GetGameMode(GetWorld()))->camera;
	if(IsValid(camera))
	{
		camera->SetFlyBoomLength();
		camera->ResetPitch();
	}
	
	// 加速以外の挙動をキャンセル
	bForce = false;
	bWire = false;
	bDive = false;	
	beforeGravityAcceleration = 0;
	currentWireMoveTime = 0;
	ResetAirbornTime();
	ResetGravityBias();
	ResetSpeedBias();
}

/// @brief グライド
void UGSFMoveComp::Graid()
{
	bGraid = true;
	SetGraidSpeedBias();
	SetGraidGravityBias();
	SetGraidStopPowerBias();
	SetGraidAirControlledBias();
	ResetAirbornTime();

	AGSFCamera* camera = Cast<AGSFGameMode>(UGameplayStatics::GetGameMode(GetWorld()))->camera;
	if(IsValid(camera))
	{
		camera->SetGraidBoomLength();
		camera->ResetPitch();
	}

	bDive = false;
}

/// @brief グライド中止
void UGSFMoveComp::GraidEnd()
{
	bGraid = false;
	ResetSpeedBias();
	ResetGravityBias();
	ResetStopPowerBias();
	ResetAirControlledBias();
	ResetAirbornTime();

	AGSFCamera* camera = Cast<AGSFGameMode>(UGameplayStatics::GetGameMode(GetWorld()))->camera;
	if(IsValid(camera) && !bFly)
	{
		camera->ResetPitch();
		camera->ResetBoomLength();
	}
}

/// @brief 指定時間で目標地点に移動
/// @param TargetPosition 目標
/// @param TimeLimit 時間
void UGSFMoveComp::WireMove(FVector TargetPosition, float TimeLimit)
{
	float distance = FVector::Distance(TargetPosition, character->GetActorLocation());
	wireMoveTime = TimeLimit;
	wireMoveSpeed = distance / TimeLimit;
	wireMoveVector = TargetPosition - character->GetActorLocation();
	wireMoveVector.Normalize();
	bWire = true;

	// Wire以外の挙動をキャンセル
	bForce = false;
	bFly = false;
	beforeGravityAcceleration = 0;
	currentWireMoveTime = 0;
	ResetAirbornTime();
	ResetGravityBias();
	ResetSpeedBias();
}

/// @brief アニメーションの移動量更新
/// @param DeltaTime
void UGSFMoveComp::UpdateRootMosion(float DeltaTime)
{
	// ルートモーションの移動量を適用
	// AnimInstanceから得られるルートモーションの移動量は
	// シーケンス内のコンポーネントスペース基準なので
	// SkeletalMeshのConver...()でワールドスペースに変換する
	USkeletalMeshComponent* mesh = character->GetMesh();
	FRootMotionMovementParams params = mesh->GetAnimInstance()->ConsumeExtractedRootMotion(1.0f);
	if (!params.bHasRootMotion)
	{
		bRootMotion = false;
		return;
	}

	// RootMotionの使用可否
	bRootMotion = true;

	// TakeAction中のみ
	if (!character->IsTakingAction())return;

	// 移動
	FHitResult* outHit = nullptr;
	FTransform tr = mesh->ConvertLocalRootMotionToWorld(params.GetRootMotionTransform());
	character->AddActorWorldTransform(tr, true, outHit);

	if (outHit->bBlockingHit)
	{
		// ルートモーション中に上に乗れちゃうことがある問題直したい
		FVector vec = FVector::ZeroVector;
	}
}

/// @brief 移動更新
/// @param DeltaTime 
void UGSFMoveComp::UpdateMove(float DeltaTime)
{
	// Wire移動中
	if (bWire)return;
	// 飛翔中
	if (bFly)return;
	// 移動制限中
	if (character->bDontMove)return;

	// 移動方向と速度
	AGSFCamera* camera = Cast<AGSFGameMode>(UGameplayStatics::GetGameMode(GetWorld()))->camera;
	if(IsValid(camera))
	{
		FVector Fwd = FVector(camera->arrow->GetForwardVector().X, camera->arrow->GetForwardVector().Y, 0);
		FVector Right = FVector(camera->arrow->GetRightVector().X, camera->arrow->GetRightVector().Y, 0);
		FVector2D V = inputBias;
		V.Normalize();
		FVector MoveVelocity = Fwd * FMath::Abs(V.Y) * inputBias.Y + Right * FMath::Abs(V.X) * inputBias.X;
		
		SweepMove(MoveVelocity, moveSpeed * moveSpeedBias * TIME);
	}
}

/// @brief 重力更新
/// @param DeltaTime 
void UGSFMoveComp::UpdateGravity(float DeltaTime)
{
	// Wire移動中
	if (bWire)return;
	// 浮遊中
	if (bFloat) return;
	// 吹っ飛び中
	if(bForce)return;
	// 飛翔中
	if(bFly)return;
	// 怯み中
	if (character->bInFear)return;

	float gravityAcceleration = 0;
	if(UnderCheck())
	{
		airborneTime = 0;
		gravityAcceleration = 0.f;
		bDive = false;
		if(!bGraid)
		{
			ResetSpeedBias();
			AGSFCamera* camera = Cast<AGSFGameMode>(UGameplayStatics::GetGameMode(GetWorld()))->camera;
			if(IsValid(camera))
			{
				camera->ResetBoomLength();
				camera->ResetPitch();
			}
		}
	}
	else
	{
		airborneTime += DeltaTime;
		if(airborneTime >= diveThreshold && !bGraid)
		{
			bDive = true;
			gravityAcceleration = diveGravity;
			SetDiveSpeedBias();
			UE_LOG(LogTemp, Log, TEXT("Dive : moveComp : UpdateGravity"));

			AGSFCamera* camera = Cast<AGSFGameMode>(UGameplayStatics::GetGameMode(GetWorld()))->camera;
			if(IsValid(camera))
			{
				camera->SetDiveBoomLength();
				camera->SetDivePitch();
			}
		}
		else
		{
			bDive = false;	
			gravityAcceleration = 0.5f * gravity * gravityBias * pow(airborneTime, 2) * TIME;

			if(!bGraid)
			{
				ResetSpeedBias();
				AGSFCamera* camera = Cast<AGSFGameMode>(UGameplayStatics::GetGameMode(GetWorld()))->camera;
				if(IsValid(camera))
				{
					camera->ResetBoomLength();
					camera->ResetPitch();
				}
			}
		}
	}
	
	// 落下速度を保存
	beforeGravityAcceleration = gravityAcceleration;

	// 移動
	SweepMove(FVector(0, 0, -1), gravityAcceleration);
}

/// @brief 吹っ飛び
void UGSFMoveComp::UpdateForce(float DeltaTime)
{
	if(!bForce)return;
	
	// 移動量
	forceValue -= forceDecelerationValue * DeltaTime;
	FVector force = forceVector;

	// 力を受けてなかったら無視
	if(forceValue <= 0)
	{
		bForce = false;
		if(bGraid)return;
		if(bFly)return;
		ResetGravityBias();
		ResetAirbornTime();
		ResetSpeedBias();
		return;
	}
	
	// 移動
	SweepMove(force, forceValue * TIME);
}

/// @brief 加速
/// @param DeltaTime
void UGSFMoveComp::UpdateFly(float DeltaTime)
{
	if(!bFly)return;

	// 方向取得
	AGSFCamera* camera = Cast<AGSFGameMode>(UGameplayStatics::GetGameMode(GetWorld()))->camera;
	if(IsValid(camera))
	{
		// カメラベクトル
		FVector Fwd = FVector(camera->arrow->GetForwardVector().X, camera->arrow->GetForwardVector().Y, 0);
		FVector Right = FVector(camera->arrow->GetRightVector().X, camera->arrow->GetRightVector().Y, 0);
		// 移動方向
		if(inputValue.IsZero())
		{
			flyVec = character->GetActorForwardVector();
		}
		else
		{
			FVector2D V = inputBias;
			V.Normalize();
			flyVec = Fwd * FMath::Abs(V.Y) * inputBias.Y + Right * FMath::Abs(V.X) * inputBias.X;
			//flyVec = Fwd * inputValue.Y + Right * inputValue.X;
		}
		
		//flyVec = camera->arrow->GetForwardVector();
		flyVec.Z = 0.f;
	}

	flyVec.Z = flyZVelocity * flyCurveZ->GetFloatValue(flyDurationTimer);
	currentFlyVelocityValue = flyVelocityValue * flyCurveSpeed->GetFloatValue(flyDurationTimer);

	DrawDebugLine(GetWorld(), character->GetActorLocation(), character->GetActorLocation() + flyVec * 300.f, FColor::Blue);

	// 時間
	flyDurationTimer += DeltaTime;
	if(flyDurationTimer > flyDuration)
	{
		bFly = false;
		ResetAirbornTime();
		ResetGravityBias();
		ResetSpeedBias();

		if(IsValid(camera))
		{
			camera->ResetBoomLength();
		}
		return;
	}
	
	// 移動
	SweepMove(flyVec, currentFlyVelocityValue * TIME);
}

/// @brief 移動方向を向く
/// @param DeltaTime
void UGSFMoveComp::UpdateRotation(float DeltaTime)
{
	// 飛翔中は自動
	if(bFly)
	{
		FRotator rotation = UKismetMathLibrary::FindLookAtRotation(character->GetActorLocation(), character->GetActorLocation() + flyVec);
		character->SetActorRotation(rotation);
		return;
	}
	
	// 入力してないときは回転しない
	if(!bInputFwd && !bInputRight)return;
	
	AGSFCamera* camera = Cast<AGSFGameMode>(UGameplayStatics::GetGameMode(GetWorld()))->camera;
	if(IsValid(camera))
	{
		// カメラベクトル
		FVector Fwd = FVector(camera->arrow->GetForwardVector().X, camera->arrow->GetForwardVector().Y, 0);
		FVector Right = FVector(camera->arrow->GetRightVector().X, camera->arrow->GetRightVector().Y, 0);
		// 移動方向
		FVector MoveVelocity = Fwd * inputValue.Y + Right * inputValue.X;
		// 回転
		FRotator rotation = UKismetMathLibrary::FindLookAtRotation(character->GetActorLocation(), character->GetActorLocation() + MoveVelocity);
		character->SetActorRotation(rotation);
	}
}

/// @brief 障害物を考慮して移動する
/// @param MoveVector 方向
/// @param MoveSpeed 速度
bool UGSFMoveComp::SweepMove(FVector MoveVector, float MoveSpeed)
{
	// 座標算出
	FVector MovePos = character->GetActorLocation();
	MovePos += MoveVector * MoveSpeed;
	// 移動
	FHitResult outHit;
	character->SetActorLocation(MovePos, true, &outHit);
	if (outHit.bBlockingHit)
	{
		// 壁に当たったら壁ずりベクトルに移動
		FVector v = FVector(MoveVector.X, MoveVector.Y, MoveVector.Z);
		v.Normalize();
		FVector alongWallVector = GetAlongWallVector(outHit.Normal, v);
		MovePos = character->GetActorLocation();
		MovePos += alongWallVector * MoveVector.Length() * MoveSpeed;
		character->SetActorLocation(MovePos, true, &outHit);
		DrawDebugLine(GetWorld(), outHit.ImpactPoint, outHit.ImpactPoint + outHit.Normal * 500.f, FColor::Red);
		DrawDebugLine(GetWorld(), character->GetActorLocation(), character->GetActorLocation() + alongWallVector * 500.f, FColor::Green);
	}
	return outHit.bBlockingHit;
}

/// @brief 壁ずりベクトルを取得
/// @param HitNormal 壁の法線
/// @param MoveVector 進行ベクトル
/// @return
FVector UGSFMoveComp::GetAlongWallVector(FVector_NetQuantizeNormal HitNormal, FVector MoveVector)
{
	// 壁ずりベクトルを算出
	// w = f - Dot(f,n) ・ n
	float dot = (MoveVector.X * HitNormal.X + MoveVector.Y * HitNormal.Y + MoveVector.Z * HitNormal.Z) /
	(FMath::Sqrt(MoveVector.X * MoveVector.X + MoveVector.Y * MoveVector.Y + MoveVector.Z * MoveVector.Z) *
		FMath::Sqrt(HitNormal.X * HitNormal.X + HitNormal.Y * HitNormal.Y + HitNormal.Z * HitNormal.Z));
	FVector alongWallVector = MoveVector - dot * HitNormal;

	return alongWallVector;
}

bool UGSFMoveComp::UnderCheck()
{
	FVector startPos = character->GetActorLocation();
	startPos.Z -= character->GetHitBox()->GetScaledCapsuleHalfHeight();
	FVector endPos = startPos - character->GetActorUpVector() * 5.f;
	FCollisionQueryParams params;
	params.AddIgnoredActor(character);
	
	DrawDebugLine(GetWorld(), startPos, endPos, FColor::Green);

	FHitResult outHit;
	GetWorld()->LineTraceSingleByChannel(outHit, startPos, endPos, ECC_WorldStatic, params);
	if (outHit.bBlockingHit)
	{
		return true;
	}
	return false;
}