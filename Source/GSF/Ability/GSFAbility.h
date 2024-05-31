// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NiagaraSystem.h"
#include "Engine/DataAsset.h"
#include "Engine/DataTable.h"

#include "GSF/Animation/MontageSectionStage.h"

#include "Misc/Paths.h"
#include "Misc/FileHelper.h"

#include "GSFAbility.generated.h"

// データテーブル用構造体
USTRUCT()
struct FAbilityData_TableRow : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	// 識別用
	UPROPERTY(EditAnywhere)
	int AbilityID = 0;
	// ダメージ
	UPROPERTY(EditAnywhere)
	float Damage = 10.f;
	// 怯み時間
	UPROPERTY(EditAnywhere)
	float FearTime = .5f;
	// 吹き飛び力
	UPROPERTY(EditAnywhere)
	float Force = 10.f;
	// 吹き飛び方向
	UPROPERTY(EditAnywhere)
	FVector ForceVector = FVector(0.f, 0.f, 0.f);
	// 前隙
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float FrontGap = 0.f;
	// 持続
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Duration = 0.f;
	// 後隙
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float RearGap = 0.f;
	// 余韻
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float AfterOver = 0.f;
};

// データ動機時の一時避難先
USTRUCT()
struct FAbilityData_TemporaryEvacuation : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
	
	// @brief アニメーションモンタージュ
	UPROPERTY()
	UAnimMontage* Montage = nullptr;
	// @brief ヒットエフェクト
	UPROPERTY()
	UNiagaraSystem* HitEffect = nullptr;
};

// 実際にゲーム側で使うキャラクターデータの構造体
USTRUCT( BlueprintType )
struct FAbilityData
{
	GENERATED_USTRUCT_BODY()

	// 識別用
	UPROPERTY(VisibleAnywhere)
	int AbilityID = 0;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString AbilityName = "";
	
	// @brief アニメーションモンタージュ
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UAnimMontage* Montage = nullptr;
	// @brief ヒットエフェクト
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UNiagaraSystem* HitEffect = nullptr;
	
	// ダメージ
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Damage = 10.f;
	// 怯み時間
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float FearTime = 1.f;
	// 吹き飛び力
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Force = 10.f;
	// 吹き飛び方向
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FVector ForceVector = FVector(0.f, 0.f, 0.f);
	// 前隙
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float FrontGap = 1.f;
	// 持続
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Duration = 1.f;
	// 後隙
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float RearGap = 1.f;
	// 余韻
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float AfterOver = 1.f;
};

// 実際にゲーム側で使うキャラクターデータの構造体
USTRUCT( BlueprintType )
struct FAbilityData_Anim
{
	GENERATED_USTRUCT_BODY()

	// 再生モーション
	UPROPERTY()
	UAnimMontage* MontageToPlay = nullptr;
	// 各セクションの持続時間
	TArray<float> Duration = {};
	// 各セクションの再生速度
	TArray<float> PlayRate = {};
	// セクションの段階
	EMontageSectionStage Stage = EMontageSectionStage::FrontGap;
};

// データ一時退避用データクラス
UCLASS()
class GSF_API UAbilityDataAsset_TemporaryEvacuation : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TMap< int, FAbilityData_TemporaryEvacuation > abilitySaveDataMap;
 
public:
	void Save(const int ID,const FAbilityData Data);
	bool Load(const int ID, FAbilityData_TemporaryEvacuation& retData)const;
};

// データアセット用クラス
UCLASS(BlueprintType)
class GSF_API UAbilityDataAsset : public UDataAsset
{
	
	GENERATED_BODY()

public:

	UAbilityDataAsset(){}
 
#if WITH_EDITORONLY_DATA

	// CSV
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UDataTable* dataTable = nullptr;
	// SaveData
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UAbilityDataAsset_TemporaryEvacuation* SaveData = nullptr;

#endif

	// アビリティデータ
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TMap< int, FAbilityData > abilityDataMap;

public:

	// データ作成用の関数
	UFUNCTION( BlueprintCallable, meta = (CallInEditor = "true") )
	void Build();
	
};