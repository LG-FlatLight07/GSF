// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "MontageSectionStage.generated.h"

// モンタージュの再生段階がどこか
UENUM()
enum class EMontageSectionStage : uint8
{
	FrontGap = 0,
	Duration,
	RearGap,
	AfterOver,

	NumStage,
};
