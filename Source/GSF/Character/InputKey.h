// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "InputKey.generated.h"

UENUM(BlueprintType)
enum class EInput : uint8
{
	None = 0,
	
	A,
	B,
	X,
	Y,
	
	L1,
	L2,
	R1,
	R2,
	
	Forward,
	Right,
	Pitch,
	Yaw,
	
	ElementsNum			UMETA(Hidden),
};

UENUM(BlueprintType)
enum class EInputType : uint8
{
	None,
	
	Single,
	Double,
	Long,
	Release,
	
	ElementsNum			UMETA(Hidden),
};