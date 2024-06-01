#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GameFramework/Pawn.h"
#include "Components/CapsuleComponent.h"

//#include "GSF/Component/GSFInputComp.h"

#include "GSFCharacter.generated.h"

class UGSFMoveComp;
class UGSFAbilityComp;
class UGSFInputComp;

/// @brief 
UENUM(BlueprintType)
enum class EInputActions : uint8
{
	None,
	
	LockOn,
	Attack,
	Bullet,
	Jump,
	AirDash,
	Glide,
	Fly,
	
	ElementsNum,
};

UENUM(BlueprintType)
enum class EAxisInputActions : uint8
{
	None,
	
	MoveForward,
	MoveRight,
	
	ElementsNum,
};

/// @brief デフォルトのキャラクタークラス
UCLASS()
class GSF_API AGSFCharacter : public APawn
{
	GENERATED_BODY()
public:
	/*------------------------------------------------------------------------------------------------------------------------------
	* 行動管理用フラグ
	-------------------------------------------------------------------------------------------------------------------------------*/
	/// @brief 被弾中
	UPROPERTY(BlueprintReadWrite)
	bool bInFear;
	/// @brief 行動不可状態にある
	UPROPERTY(BlueprintReadWrite)
	bool bDontMove;
	/// @brief なにかしらのアクションをしているか(ガードやステップ、技など)
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool bTakingAction;
	
protected:
	/*------------------------------------------------------------------------------------------------------------------------------
	* コンポーネント
	-------------------------------------------------------------------------------------------------------------------------------*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USkeletalMeshComponent* mesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCapsuleComponent* hitBox;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UGSFMoveComp* moveComp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UGSFAbilityComp* abilityComp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UGSFInputComp* inputComp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UGSFAnimComp* animComp;

	
public:
	AGSFCharacter();
	virtual void Tick(float DeltaTime) override;
	
	/*------------------------------------------------------------------------------------------------------------------------------
	* コンポーネント取得
	-------------------------------------------------------------------------------------------------------------------------------*/
	// メッシュ取得
	UFUNCTION(BlueprintCallable, BlueprintPure)
	USkeletalMeshComponent* GetMesh()const { return mesh; }
	// コリジョン取得
	UFUNCTION(BlueprintCallable, BlueprintPure)
	UCapsuleComponent* GetHitBox()const { return hitBox; }
	UFUNCTION(BlueprintCallable, BlueprintPure)
	UGSFAbilityComp* AbilityComp()const{return abilityComp;}
	UFUNCTION(BlueprintCallable, BlueprintPure)
	UGSFMoveComp* MoveComp()const{return moveComp;}
	UFUNCTION(BlueprintCallable, BlueprintPure)
	UGSFInputComp* InputComp()const{return inputComp;}
	UFUNCTION(BlueprintCallable, BlueprintPure)
	UGSFAnimComp* AnimComp()const{return animComp;}
	
	/*------------------------------------------------------------------------------------------------------------------------------
	* 行動制御
	-------------------------------------------------------------------------------------------------------------------------------*/
	/// @brief 移動 前後
	void MoveForward(const float MoveValue);
	/// @brief 移動 左右
	void MoveRight(const float MoveValue);
	/// @brief InputSystemComponentから入力を受け取る
	UFUNCTION()
	void TakeAction(enum EInputActions Action);
	/// @brief 何かしらのアクションをしているか
	bool IsTakingAction()const;
	/// @brief ダメージを受ける
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void RecieveDamage();
	void RecieveDamage_Implementation();
	/// @brief 怯み
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void Fear();
	void Fear_Implementation();
	/// @brief 死亡
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void Death();
	void Death_Implementation();
	/// @brief グライド
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void Graid(const bool key);
	void Graid_Implementation(const bool key);
	/// @brief 空中ダッシュ
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void AirDash();
	void AirDash_Implementation();
	/// @brief 空中ジャンプ
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void AirJump();
	void AirJump_Implementation();
	/// @brief 飛翔
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void Fly();
	void Fly_Implementation();
	/// @brief ロックオン
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void ManualAim(const bool look);
	void ManualAim_Implementation(const bool look);
	
	/*------------------------------------------------------------------------------------------------------------------------------
	* 入力制御
	-------------------------------------------------------------------------------------------------------------------------------*/
	/// @brief 入力メソッド群のバインド
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

protected:
	virtual void BeginPlay() override;
};