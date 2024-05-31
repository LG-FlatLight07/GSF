// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GSF/Component/GSFComponentInterface.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GSF/Character/ActionExecutor.h"
#include "GSF/Character/GSFCharacter.h"
#include "GSFMoveComp.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class GSF_API UGSFMoveComp : public UCharacterMovementComponent, public IGSFComponentInterface
{
	GENERATED_BODY()

private:
	
	UPROPERTY()
	AGSFCharacter* character;

	// 空中にいる
	bool bAir;

	// 入力値
	FVector input;

	// 接地判定レイの長さ
	UPROPERTY(EditAnywhere, Category = "MechaMovement")
	float rayCastLengthFromAir;

	// 空中でルートモーション実行中の、入力による移動量の補正値
	UPROPERTY(EditAnywhere, Category = "MechaMovement")
	float correctRootVelocityBiasByInput;

	// @brief スピード
	UPROPERTY(EditAnywhere, Category = "MechaMovement")
	float moveVelocity = 70.f;

	// @brief 摩擦
	UPROPERTY(EditAnywhere, Category = "MechaMovement")
	float friction = 7.f;
	
	// @brief ジャンプ力
	UPROPERTY(EditAnywhere, Category = "MechaMovement")
	float jumpPower = 9.3f;

	// @brief 大ジャンプ力
	UPROPERTY(EditAnywhere, Category = "MechaMovement")
	float bigJumpPower = 18.f;

	// @brief ジャンプ力
	UPROPERTY(EditAnywhere, Category = "MechaMovement")
	float dodgePower = 9.3f;

	// @brief 大ジャンプ力
	UPROPERTY(EditAnywhere, Category = "MechaMovement")
	float bigDodgePower = 18.f;
	
	// @brief 重力
	UPROPERTY(EditAnywhere, Category = "MechaMovement")
	float gravity = 1.f;

protected:

	virtual void BeginPlay() override;

public:

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

#pragma region CharacterMovement

	virtual void AddInputVector(FVector WorldVector, bool bForce) override;
	
	virtual void AddForce(FVector Force) override;

	virtual FVector ConstrainAnimRootMotionVelocity(const FVector& RootMotionVelocity, const FVector& CurrentVelocity) const override;

	virtual bool IsFalling() const override;

protected:

	/// @brief 物理挙動の当たり制御
	virtual void PhysWalking(float deltaTime, int32 Iterations) override;

#pragma endregion 
	
public:

	/// @brief characterにメソッドを登録する
	/// @param Executor characterのメソッド実行用クラス
	virtual  void RegisterMethods(UMethodExecutor* Executor) override;
	
private:
	
	/// @brief 接地判定
	void UpdateIsInAir();

	/// @brief 前後移動
	/// @param Value
	void MoveForward(float Value);
	
	/// @brief 左右移動
	/// @param Value
	void MoveRight(float Value);

	/// @brief 
	void Jump();

	/// @brief 
	void HighJump();

	/// @brief 
	void Dodge();

	/// @brief 
	void LongDodge();

	/// @brief 
	void Fly();

	/// @brief 
	void Glide();
	
	/// @brief 
	void Landing();
	
};
