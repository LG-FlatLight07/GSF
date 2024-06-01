#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GSF/Character/GSFCharacter.h"
#include "GSFMoveComp.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class GSF_API UGSFMoveComp : public UActorComponent
{
	GENERATED_BODY()

public:
	
	/// @brief コンポーネントの保持キャラクター
	UPROPERTY(BlueprintReadOnly)
	AGSFCharacter* character;

	UPROPERTY(EditAnywhere)
	UCurveFloat* flyCurveZ;
	UPROPERTY(EditAnywhere)
	UCurveFloat* flyCurveSpeed;
	
protected:
	/*------------------------------------------------------------------------------------------------------------------------------
	* 各フラグ
	-------------------------------------------------------------------------------------------------------------------------------*/
	/// @brief ルートモーション適用中
	bool bRootMotion = false;
	/// @brief 浮遊
	UPROPERTY(BlueprintReadOnly)
	bool bFloat;
	/// @brief Wire移動中
	UPROPERTY(BlueprintReadOnly)
	bool bWire;
	/// @brief 吹っ飛び中
	UPROPERTY(BlueprintReadOnly)
	bool bForce;
	/// @brief 加速中
	UPROPERTY(BlueprintReadOnly)
	bool bFly;
	/// @brief グライド中
	UPROPERTY(BlueprintReadOnly)
	bool bGraid;
	/// @brief 急降下中
	UPROPERTY(BlueprintReadOnly)
	bool bDive;
	
	/*------------------------------------------------------------------------------------------------------------------------------
	* 計算用
	-------------------------------------------------------------------------------------------------------------------------------*/
	
	/// @brief 滞空時間
	float airborneTime;
	/// @brief 前フレーム落下速度
	float beforeGravityAcceleration;
	
	/// @brief 吹っ飛び方向
	FVector forceVector;
	/// @brief 吹っ飛び力
	float forceValue;
	/// @brief 吹っ飛びに対する減速力
	float forceDecelerationValue;

	/// @brief 現在の飛翔加速力
	float currentFlyVelocityValue;
	/// @brief 持続時間タイマー
	float flyDurationTimer;
	/// @brief 持続
	FVector flyVec;
	
	/// @brief Wire移動方向
	FVector wireMoveVector;
	/// @brief Wire移動速度
	float wireMoveSpeed;
	/// @brief Wire移動時間
	float wireMoveTime;
	/// @brief Wire移動時間
	float currentWireMoveTime;

	/// @brief 最後の入力ベクトル
	FVector2D beforeMoveVector;
	/// @brief 前フレーム位置
	FVector beforePos;
	
public:
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @brief 入力値
	UPROPERTY(BlueprintReadOnly)
	FVector2D inputValue;
	/// @brief 入力値の補正値
	FVector2D inputBias;
	/// @brief 入力
	bool bInputFwd;
	/// @brief 入力
	bool bInputRight;
	// @brief 移動速度
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float moveSpeed = 100.f;
	// @brief 移動速度倍率
	UPROPERTY(BlueprintReadOnly)
	float moveSpeedBias = 1.f;
	// @brief グライド移動速度倍率
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float graidSpeedBias = 1.7f;
	// @brief 急降下移動速度倍率
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float diveSpeedBias = 3.5f;
	// @brief 空中ダッシュスピード
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float airDashSpeed = 20.f;
	// @brief 空中ダッシュ減衰
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float airDashDeceleration = 14.f;
	// @brief 空中ジャンプパワー
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float airJumpPower = 20.f;
	// @brief 空中ジャンプ減衰
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float airJumpDeceleration = 14.f;
	/// @brief 制動
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float airControll = 0.02f;
	/// @brief 制動倍率
	UPROPERTY(BlueprintReadOnly)
	float airControllBias = 1.f;
	/// @brief グライド制動倍率
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float graidAirControllBias = 1.5f;
	/// @brief 停止力
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float stopPower = 0.01f;
	/// @brief 停止力倍率
	UPROPERTY(BlueprintReadOnly)
	float stopPowerBias = 1.f;
	/// @brief グライド停止力倍率
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float graidStopPowerBias = 1.5f;
	// @brief 減速力
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float friction = 7.f;
	// @brief 落下速度
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float gravity = 5.f;
	// @brief 落下速度倍率
	UPROPERTY(BlueprintReadOnly)
	float gravityBias = 1.f;
	// @brief グライド落下速度倍率
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float graidGravityBias = 0.2f;
	// @brief 急降下落下速度
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float diveGravity = 20.f;
	// @brief 急降下移行時間
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float diveThreshold = 5.f;
	/// @brief 飛翔加速力
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float flyVelocityValue = 5.f;
	/// @brief 飛翔加速力
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float flyZVelocity = 10.f;
	/// @brief 飛翔持続時間
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float flyDuration = 3.f;
	/// @brief 振り向き速度
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float rotationSpeed = 15.f;
	
public:
	UGSFMoveComp();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void SetGraidSpeedBias(){moveSpeedBias = graidSpeedBias;}
	void SetDiveSpeedBias(){moveSpeedBias = diveSpeedBias;}
	void ResetSpeedBias(){moveSpeedBias = 1.f;}
	void SetGraidGravityBias(){gravityBias = graidGravityBias;}
	void ResetGravityBias(){gravityBias = 1.f;}
	void SetGraidStopPowerBias(){stopPowerBias = graidStopPowerBias;}
	void ResetStopPowerBias(){stopPowerBias = 1.f;}
	void SetGraidAirControlledBias(){airControllBias = graidAirControllBias;}
	void ResetAirControlledBias(){airControllBias = 1.f;}
	// 滞空時間リセット
	void ResetAirbornTime(){airborneTime = 0.f;}
	
	/// @brief 前後方向入力
	/// @param InputValue
	void AddForwardMovementInput(float InputValue);
	/// @brief 左右方向入力
	/// @param InputValue
	void AddRightMovementInput(float InputValue);
	/// @brief 力を加える
	/// @param Vector 方向ベクトル
	/// @param Value 吹っ飛び力
	/// @param Deceleration 減速力
	UFUNCTION(BlueprintCallable)
	void AddForce(FVector Vector, float Value, float Deceleration);
	/// @brief 空中回避
	UFUNCTION(BlueprintCallable)
	void AirDash();
	/// @brief 空中ジャンプ
	UFUNCTION(BlueprintCallable)
	void AirJump();
	/// @brief 加速していく
	UFUNCTION(BlueprintCallable)
	void Fly();
	/// @brief グライド
	UFUNCTION(BlueprintCallable)
	void Graid();
	/// @brief グライド中止
	UFUNCTION(BlueprintCallable)
	void GraidEnd();
	/// @brief 指定時間で目標地点に移動
	/// @param TargetPosition 目標
	/// @param TimeLimit 時間
	UFUNCTION(BlueprintCallable)
	void WireMove(FVector TargetPosition, float TimeLimit);
	/// @brief 浮遊
	UFUNCTION(BlueprintCallable)
	void SetFloat(const bool Float){bFloat = Float;airborneTime = 0;}

protected:

	virtual void BeginPlay() override;
	
	/// @brief アニメーションの移動量更新
	/// @param DeltaTime
	void UpdateRootMosion(float DeltaTime);
	/// @brief 移動更新
	/// @param DeltaTime 
	void UpdateMove(float DeltaTime);
	/// @brief 重力更新
	/// @param DeltaTime 
	void UpdateGravity(float DeltaTime);
	/// @brief 吹っ飛び
	/// @param DeltaTime
	void UpdateForce(float DeltaTime);
	/// @brief 加速
	/// @param DeltaTime
	void UpdateFly(float DeltaTime);
	/// @brief 移動方向を向く << 相手キャラクターの方を向く
	/// @param DeltaTime
	void UpdateRotation(float DeltaTime);

	/// @brief 障害物を考慮して移動する
	/// @param MoveVector 方向
	/// @param MoveSpeed 速度
	bool SweepMove(FVector MoveVector, float MoveSpeed);
	/// @brief 壁沿いのベクトルを取得
	/// @param HitNormal 障害物の法線ベクトル
	/// @param MoveVector 移動方向
	/// @return 壁沿いのベクトル
	FVector GetAlongWallVector(FVector_NetQuantizeNormal HitNormal, FVector MoveVector);

	bool UnderCheck();
};