
#pragma once

#include "CoreMinimal.h"

#include "UObject/Object.h"

#include "GSFCharacterInterface.generated.h"

class UMechaAbilityComp;
class UMechaAnimComp;
class UMechaInputComp;
class UMechaMoveComp;

UINTERFACE(MinimalAPI, meta = (CannotImplementInterfaceInBlueprint))
class UGSFCharacterInterface : public UInterface
{
	GENERATED_BODY()
};

class GSF_API IGSFCharacterInterface
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintCallable)
	virtual UGSFAbilityComp* GetAbilityComp() = 0;
	UFUNCTION(BlueprintCallable)
	virtual UGSFAnimComp* GetAnimComp() = 0;
	UFUNCTION(BlueprintCallable)
	virtual UGSFInputComp* GetInputComp() = 0;
	UFUNCTION(BlueprintCallable)
	virtual UGSFMoveComp* GetMoveComp() = 0;
	
};

