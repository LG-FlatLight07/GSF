
#include "GSF/Component/GSFAbilityComp.h"
#include "GSF/Component/GSFAnimComp.h"
#include "GSF/Ability/GSFAbility.h"
#include "Kismet/GameplayStatics.h"

UGSFAbilityComp::UGSFAbilityComp()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UGSFAbilityComp::BeginPlay()
{
	Super::BeginPlay();
	character = Cast<AGSFCharacter>(GetOwner());
}

void UGSFAbilityComp::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UGSFAbilityComp::RegisterMethods(UMethodExecutor* Executor)
{
	Executor->AddActionMethod([this](const EAction Action, const float& ReactionTime){ExecuteAction(Action, ReactionTime);});
}

// キーから実行
void UGSFAbilityComp::ExecuteAction(const EAction Action, const float& ReactionTime)
{
	if(!abilityData.Contains(Action)) return;
	
	// 再生
	if(!IsValid(character->animComp))return;
	if(character->animComp->TryPlayMontage(abilityData[Action], 0.f, ReactionTime, 0.f, 0.f))
	{
		// 再生中の技データを保管
		SetActiveAbilityData(abilityData[Action]);
	}
	else
	{
		activeAbilityData = {};
	}
}

// 実行中のデータを取得
FAbilityData UGSFAbilityComp::GetActiveAbilityData() const
{
	return activeAbilityData;
}

// 実行中の技データを設定
void UGSFAbilityComp::SetActiveAbilityData(const FAbilityData& Data)
{
	activeAbilityData = {};
	activeAbilityData.AbilityID = Data.AbilityID;
	activeAbilityData.AbilityName = Data.AbilityName;
	activeAbilityData.Damage = Data.Damage;
	activeAbilityData.FearTime = Data.FearTime;
	activeAbilityData.Force = Data.Force;
	activeAbilityData.ForceVector = Data.ForceVector;
}