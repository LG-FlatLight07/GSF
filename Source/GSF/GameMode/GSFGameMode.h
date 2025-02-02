// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "GSF/Camera/GSFCamera.h"
#include "GSFGameMode.generated.h"

UCLASS()
class GSF_API AGSFGameMode : public AGameMode
{
	GENERATED_BODY()

public:

protected:
	UFUNCTION()
	virtual void BeginPlay() override;
};
