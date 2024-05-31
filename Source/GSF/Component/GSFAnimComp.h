// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Components/ActorComponent.h"
#include "GSF/Character/GSFCharacter.h"
#include "GSF/Ability/GSFAbility.h"
#include "GSFAnimComp.generated.h"

struct FAbilityData;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class GSF_API UGSFAnimComp : public UActorComponent
{
	GENERATED_BODY()

private:

	/// @brief コンポーネントの保持キャラクター
	UPROPERTY()
	AGSFCharacter* character;
	
	/// @brief アニメーションインスタンス
	UPROPERTY()
	UAnimInstance* animInstance;

	/// @brief 実行中の技データ
	FAbilityData_Anim data;

public:
	
	UGSFAnimComp();

protected:

	virtual void BeginPlay() override;

public:

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	
	/// @brief アニメーション再生
	/// @param Data アニメーションデータ
	/// @param FrontTime 前隙の再生時間
	/// @param DurationTime 持続の再生時間
	/// @param RearTime 後隙の再生時間
	/// @param AfterTime 余韻の再生時間
	bool TryPlayMontage(const FAbilityData& Data, float FrontTime = 0.f, float DurationTime = 0.f, float RearTime = 0.f, float AfterTime = 0.f);

private:

	/// @brief 再生するアニメーション取得
	UAnimMontage* TryGetMontageToPlay(const FAbilityData& Data)const;

	/// @brief アニメーションの再生速度を取得
	float GetPlayRate(const uint8 SectionIndex);

	/// @brief モンタージュ通知
	UFUNCTION()
	void OnMontageSectionStart(FName _notifyName, const FBranchingPointNotifyPayload& _branchingPointNotifyPayload);

};
