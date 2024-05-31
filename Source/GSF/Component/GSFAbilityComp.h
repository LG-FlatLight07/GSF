
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "GSF/Character/GSFCharacter.h"
#include "GSF/Component/GSFComponentInterface.h"
#include "GSF/Ability/GSFAbility.h"

#include "GSFAbilityComp.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class GSF_API UGSFAbilityComp : public UActorComponent, public IGSFComponentInterface
{
	GENERATED_BODY()

protected:
	
	/// @brief 装備中の技データ
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<EAction, FAbilityData> abilityData = {};

private:

	/// @brief コンポーネントの保持者
	UPROPERTY()
	AGSFCharacter* character;

	/// @brief 実行中の技データ
	UPROPERTY(VisibleAnywhere)
	FAbilityData activeAbilityData = {};

public:
	
	UGSFAbilityComp();

protected:
	
	virtual void BeginPlay() override;

public:

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:

	/// @brief characterにメソッド登録
	virtual  void RegisterMethods(UMethodExecutor* Executor) override;

	/// @brief アビリティ発動
	/// @param Action 入力
	/// @param ReactionTime 入力
	UFUNCTION(BlueprintCallable, Category = "MechaAbility")
	void ExecuteAction(const EAction Action, const float& ReactionTime = 0.f);
	
	/// @brief 実行中の技データを取得
	UFUNCTION(BlueprintCallable)
	FAbilityData GetActiveAbilityData()const;

private:

	// 実行する技データを設定
	void SetActiveAbilityData(const FAbilityData& Data);
	
};
