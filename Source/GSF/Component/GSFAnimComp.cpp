// Fill out your copyright notice in the Description page of Project Settings.


#include "GSF/Component/GSFAnimComp.h"
#include "GSF/Component/GSFMoveComp.h"
#include "GSF/Animation/MontageSectionStage.h"

UGSFAnimComp::UGSFAnimComp()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UGSFAnimComp::BeginPlay()
{
	Super::BeginPlay();

	character = Cast<AGSFCharacter>(GetOwner());
}

void UGSFAnimComp::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

bool UGSFAnimComp::TryPlayMontage(const FAbilityData& Data, float FrontTime, float DurationTime, float RearTime, float AfterTime)
{
	// 初期化
	data = FAbilityData_Anim();
	// データ取得
	data.MontageToPlay = TryGetMontageToPlay(Data);
	
	if(!data.MontageToPlay || !animInstance || !character)return false;

	// 各セクション間の再生時間取得
	if(FrontTime != 0.f)
		data.Duration.Add(FrontTime);
	else
		data.Duration.Add(Data.FrontGap);
	if(DurationTime != 0.f)
		data.Duration.Add(DurationTime);
	else
		data.Duration.Add(Data.Duration);
	if(RearTime != 0.f)
		data.Duration.Add(RearTime);
	else
		data.Duration.Add(Data.RearGap);
	if(AfterTime != 0.f)
		data.Duration.Add(AfterTime);
	else
		data.Duration.Add(Data.AfterOver);
	
	// 各セクション間の再生速度取得
	if(data.MontageToPlay->CompositeSections.Num() == 1)data.PlayRate.Add(1.f);// Setting 'DefaultSection' Only
	for (uint8 SectionIndex = 0; SectionIndex < data.MontageToPlay->CompositeSections.Num() - 1; ++SectionIndex)
	{
		data.PlayRate.Add(GetPlayRate(SectionIndex));
	}
	
	// デリゲートのバインドし直し
	// 本当はセクションの開始にバインドしたい
	animInstance->OnPlayMontageNotifyBegin.RemoveDynamic(this, &UGSFAnimComp::OnMontageSectionStart);
	animInstance->OnPlayMontageNotifyBegin.AddDynamic(this, &UGSFAnimComp::OnMontageSectionStart);
	
	// 再生
	animInstance->Montage_Play(data.MontageToPlay, data.PlayRate[static_cast<uint8>(EMontageSectionStage::FrontGap)]);
	// セクションの段階をカウント
	data.Stage = EMontageSectionStage::Duration;
	
	return true;
}

UAnimMontage* UGSFAnimComp::TryGetMontageToPlay(const FAbilityData& Data) const
{
	return Data.Montage;
}

float UGSFAnimComp::GetPlayRate(const uint8 SectionIndex)
{
	if(data.Duration.Num() <= SectionIndex)return 1.f;
	
	// セクション取得
	FCompositeSection section = data.MontageToPlay->CompositeSections[SectionIndex];
		
	// セクションのトータル時間
	float sectionStartTime;
	float sectionEndTime;
	data.MontageToPlay->GetSectionStartAndEndTime(SectionIndex, sectionStartTime, sectionEndTime);
	float sectionLength = sectionEndTime - sectionStartTime;

	// 設定されてなかったら等倍で再生
	return (data.Duration[SectionIndex] == 0.f) ? 1.f : sectionLength / data.Duration[SectionIndex];
}

void UGSFAnimComp::OnMontageSectionStart(FName _notifyName, const FBranchingPointNotifyPayload& _branchingPointNotifyPayload)
{
	if(_notifyName != "SectionNotify")return;

	// 再生速度を変更
	if(data.Stage == EMontageSectionStage::Duration)
	{
		animInstance->Montage_SetPlayRate(data.MontageToPlay, data.PlayRate[static_cast<uint8>(EMontageSectionStage::Duration)]);
		data.Stage = EMontageSectionStage::RearGap;
	}
	else if(data.Stage == EMontageSectionStage::RearGap)
	{
		animInstance->Montage_SetPlayRate(data.MontageToPlay, data.PlayRate[static_cast<uint8>(EMontageSectionStage::RearGap)]);
		data.Stage = EMontageSectionStage::AfterOver;
	}
	else if(data.Stage == EMontageSectionStage::AfterOver)
	{
		animInstance->Montage_SetPlayRate(data.MontageToPlay, data.PlayRate[static_cast<uint8>(EMontageSectionStage::AfterOver)]);
	}
}

