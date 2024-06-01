#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"

#include "ActionType.h"
#include "InputKey.h"
#include "ActionExecutor.h"
#include "GSFCharacterInterface.h"
#include "GSF/Ability/GSFAbility.h"

#include "GSFCharacter.generated.h"

class AGSFCamera;
class UGSFAbilityComp;
class UGSFAnimComp;
class UGSFMoveComp;
class UGSFInputComp;

UCLASS()
class GSF_API AGSFCharacter : public ACharacter, public IGSFCharacterInterface
{
	GENERATED_BODY()

public:

	// コンポーネント
	UPROPERTY()
	UMethodExecutor* executor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MechaCharacter")
	UGSFMoveComp* moveComp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MechaCharacter")
	UGSFInputComp* inputComp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MechaCharacter")
	UGSFAnimComp* animComp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MechaCharacter")
	UGSFAbilityComp* abilityComp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MechaCharacter")
	AGSFCamera* cameraComp;

protected:

	/// @brief hp
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	float health;
	
	/// @brief max hp
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float maxHealth;

private:
	
	/// @brief アクション中
	UPROPERTY(VisibleAnywhere)
	bool bTakeAction = false;

	/// @brief 空中にいる
	UPROPERTY(VisibleAnywhere)
	bool bInAir = false;

	/// @brief グライド中
	UPROPERTY(VisibleAnywhere)
	bool bGlide = false;

	/// @brief 飛行中
	UPROPERTY(VisibleAnywhere)
	bool bFly = false;

	/// @brief 急降下中
	UPROPERTY(VisibleAnywhere)
	bool bDive = false;

	/// @brief 全入力遮断
	UPROPERTY(VisibleAnywhere)
	bool bCantInput = false;

	/// @brief 移動入力を受け付けるか
	UPROPERTY(VisibleAnywhere)
	bool bCantMove = false;

	/// @brief カメラ入力を受け付けるか
	UPROPERTY(VisibleAnywhere)
	bool bCantCamera = false;

	/// @brief 先行入力を受け付けるか
	UPROPERTY(VisibleAnywhere)
	bool bCantBufferedInput = false;
	
public:

	AGSFCharacter(const FObjectInitializer& ObjectInitializer);

protected:
	
	virtual void BeginPlay() override;

public:

	virtual void Tick(float DeltaTime) override;
	
public:

	/// @brief ラウンドごとの開始処理(現状 回復＆位置リセット)
	UFUNCTION(BlueprintCallable, Category = "MechaCharacter")
	void RestartPlayer();
	
	/// @brief 体力を減らす
	/// @param Data ダメージ
	UFUNCTION(BlueprintCallable, Category = "MechaCharacter")
	void SubtractHealth(const FAbilityData& Data);
	
	/// @brief 被弾
	/// @param Data ダメージ源
	UFUNCTION(BlueprintCallable, Category = "MechaCharacter")
	bool ReceiveDamage(const FAbilityData& Data);

	/// @brief アクション中
	UFUNCTION(BlueprintCallable, Category = "MechaCharacter")
	void SetTakeAction(bool action){bTakeAction = action;}
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "MechaCharacter")
	bool IsTakingAction()const{return bTakeAction;}

	/// @brief 空中
	UFUNCTION(BlueprintCallable, Category = "MechaCharacter")
	void SetIsInAir(bool air){bInAir = air;}
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "MechaCharacter")
	bool IsInAir()const{return bInAir;}

	/// @brief グライド
	UFUNCTION(BlueprintCallable, Category = "MechaCharacter")
	void SetIsGlide(bool glide){bGlide = glide;}
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "MechaCharacter")
	bool IsGlide()const{return bGlide;}

	/// @brief 飛行
	UFUNCTION(BlueprintCallable, Category = "MechaCharacter")
	void SetIsFly(bool fly){bFly = fly;}
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "MechaCharacter")
	bool IsFly()const{return bFly;}

	/// @brief 急降下
	UFUNCTION(BlueprintCallable, Category = "MechaCharacter")
	void SetIsDive(bool dive){bDive = dive;}
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "MechaCharacter")
	bool IsDive()const{return bDive;}
	
	/// @brief 移動入力を受け付けるか
	UFUNCTION(BlueprintCallable, Category = "MechaCharacter")
	void SetCantMove(bool move){bCantMove = move;}
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "MechaCharacter")
	bool IsCantMove()const{return bCantMove;}

	/// @brief カメラ入力を受け付けるか
	UFUNCTION(BlueprintCallable, Category = "MechaCharacter")
	void SetCantCamera(bool camera){bCantCamera = camera;}
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "MechaCharacter")
	bool IsCantCamera()const{return bCantCamera;}

	/// @brief 先行入力を受け付けるか
	UFUNCTION(BlueprintCallable, Category = "MechaCharacter")
	void SetCantBufferedInput(bool input){bCantBufferedInput = input;}
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "MechaCharacter")
	bool IsCantBufferedInput()const{return bCantBufferedInput;}
	
	/// @brief キャラクターの入力許可状態をリセット
	UFUNCTION(BlueprintCallable, Category = "MechaCharacter")
	void ResetCharacterInputPermission();
	
	// コンポーネント取得
	UFUNCTION(BlueprintCallable)
	virtual UGSFAbilityComp* GetAbilityComp() override{return abilityComp;}
	UFUNCTION(BlueprintCallable)
	virtual UGSFAnimComp* GetAnimComp() override{return animComp;}
	UFUNCTION(BlueprintCallable)
	virtual UGSFMoveComp* GetMoveComp() override{return moveComp;}
	UFUNCTION(BlueprintCallable)
	virtual UGSFInputComp* GetInputComp() override{return inputComp;}

	// カプセルの大きさ取得
	float GetCapsuleHalfHeight()const{return GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();};
	
private:

	/// @brief 行動用イベントメソッド
	UFUNCTION()
	void InputHandler(EInput Input, EInputType Type);

	/// @brief 行動用イベントメソッド
	UFUNCTION()
	void AxisInputHandler(EInput Input, EInputType Type, float Value);

};