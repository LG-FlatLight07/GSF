
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GSF/Character/GSFCharacter.h"
#include "GSFInputComp.generated.h"

/// @brief 
UENUM(BlueprintType)
enum class EInputKey : uint8
{
	None,
	
	Mouse_Right,
	Mouse_Right_WithAxis,
	
	ElementsNum,
};

UENUM(BlueprintType)
enum class EAxisInputKey : uint8
{
	None,
	
	WS,
	AD,
	
	ElementsNum,
};

UENUM(BlueprintType)
enum class EInputType : uint8
{
	None,
	
	Single,
	Double,
	Long,
	
	ElementsNum,
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GSF_API UGSFInputComp : public UActorComponent
{
	GENERATED_BODY()
private:
	/// @brief コンポーネントの保持キャラクター
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
	EInputActions WaitingAction;

	/// @brief 二回入力を待っている
	bool bWaitForDoubleInput;
	/// @brief 二回入力における一回目の入力値
	EInputActions beforeInputAction;
	/// @brief 二回入力の猶予時間
	UPROPERTY(EditAnywhere)
	float waitForDoubleInputTimeLimit = 0.3f;
	/// @brief 二回入力の猶予時間をカウントする
	float doubleInputTimer;

	/// @brief 二回入力の猶予時間
	UPROPERTY(EditAnywhere)
	float waitForGraidInputTimeLimit = 1.2f;
	/// @brief 二回入力の猶予時間をカウントする
	float graidInputTimer;
	
public:
	// 長押し
	UPROPERTY(BlueprintReadOnly)
	bool bPresseGraidKey;
	UPROPERTY(BlueprintReadOnly)
	bool bPresseBulletKey;
	UPROPERTY(BlueprintReadOnly)
	bool bConcentrationKey;
	UPROPERTY(BlueprintReadOnly)
	bool bPressedLookOnKey;
	// 単押し
	UPROPERTY(BlueprintReadOnly)
	bool bFly;
	
public:	
	UGSFInputComp();
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/// @brief 入力メソッド群のバインド
	void SetupInputComp(UInputComponent* PlayerInputComponent);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:
	/// @brief 先行入力
	void BufferedInputCount(float DeltaTime);
	/// @brief 実行待ちのアクションを実行する
	void ExecutePendingActions();
	/// @brief 何かのアクションを待機中にする
	void WaitForAction(EInputActions Action);
	/// @brief アクションの待機を解除する
	void StopWaitForAction();

	/// @brief 二回入力の判定
	bool DoubleInputCheck(const EInputActions Action, const EInputActions ExAction);
	/// @brief 二回入力の猶予カウント
	void DoubleInputCount(const float DeltaTime);

	/// @brief 長押しの判定
	void LongInputCheck(const float DeltaTime, const float Limit, float& Counter, bool& Action);

	void EventGraid();
	
	void InputAxis_Pitch(const float Value);
	void InputAxis_Yaw(const float Value);
	void InputAxis_MoveForward(const float Value);
	void InputAxis_MoveRight(const float Value);
	void InputAction_LookOn();
	void InputAction_LookOn_Released();
	void InputAction_Attack();
	void InputAction_AirDash();
	void InputAction_AirDash_Released();
	void InputAction_Bullet();
	void InputAction_Bullet_Released();
	void InputAction_Concentration();
	void InputAction_Concentration_Released();
};
