
#pragma once

#include "CoreMinimal.h"
#include "AnimNotifyState_TimedNiagaraEffect.h"
#include "TimedNiagaraSystem.generated.h"

UCLASS()
class GSF_API UTimedNiagaraSystem : public UAnimNotifyState_TimedNiagaraEffect
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = NiagaraSystem, meta = (ToolTip = "The niagara system to spawn for the notify state"))
	TObjectPtr<UNiagaraSystem> OtherSystem;

public:
	UFUNCTION(BlueprintCallable)
	void SetEfect()
	{
		Template = OtherSystem;
	}
};
