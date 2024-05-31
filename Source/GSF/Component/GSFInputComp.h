
#pragma once

#include "CoreMinimal.h"
#include "GSF/Character/InputKey.h"
#include "GSF/Character/GSFCharacter.h"
#include "Components/ActorComponent.h"
#include "GSFInputComp.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class GSF_API UGSFInputComp : public UActorComponent
{
	GENERATED_BODY()

	DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnInputEvent, EInput, Input, EInputType, Type);
	DECLARE_DYNAMIC_DELEGATE_ThreeParams(FOnAxisInputEvent, EInput, Input, EInputType, Type, float, Value);
	
public:

	UPROPERTY()
	FOnInputEvent onInputEvent;
	UPROPERTY()
	FOnAxisInputEvent onAxisInputEvent;

private:

	/// @brief キャラクター
	UPROPERTY()
	AGSFCharacter* character;
	
	/// @brief なんらかのアクションを待機中
    bool bWaitingAction;
    /// @brief 先行入力されてから実行を待機できる時間
    UPROPERTY(EditAnywhere)
    float timeToWait_BufferedInputExecution = 0.6f;
    /// @brief 先行入力されてから実行を待機できる時間をカウント
    float bufferedInputTimer;
    /// @brief 待機中のアクション
	TPair<EInput, EInputType> WaitingAction;

    /// @brief 二回入力を待っている
    bool bWaitForDoubleInput;
    /// @brief 一回目の入力値
    EInput firstInputAction;
    /// @brief 二回入力の猶予時間
    UPROPERTY(EditAnywhere)
    float waitForDoubleInputTimeLimit = 0.3f;
    /// @brief 二回入力の猶予時間をカウントする
    float doubleInputTimer;
	
public:
	
	UGSFInputComp();

protected:

	virtual void BeginPlay() override;

public:

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:

	/// @brief 入力バインド
	void InputBind(UInputComponent* InputComponent);

	/// @brief 先行入力
	void BufferedInputCount(float DeltaTime);
	/// @brief 実行待ちのアクションを実行する
	void ExecutePendingActions();
	/// @brief 何かのアクションを待機中にする
	void WaitForAction(const EInput Input, const EInputType Type);
	/// @brief アクションの待機を解除する
	void StopWaitForAction();

	/// @brief 二回入力の判定
	bool DoubleInputCheck(const EInput Action);
	/// @brief 二回入力の猶予カウント
	void DoubleInputCount(float DeltaTime);

	/// @brief 入力試行
	void Input(const EInput Action, const EInputType Type);

	void A();
	void B();
	void X();
	void Y();
	void L1();
	void L2();
	void R1();
	void R2();
	void L1_();
	void L2_();
	void R1_();
	void R2_();

	void Forward(const float Value);
	void Right(const float Value);
	void Pitch(const float Value);
	void Yaw(const float Value);
};
