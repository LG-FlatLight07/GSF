// Fill out your copyright notice in the Description page of Project Settings.


#include "GSFCamera.h"

#include "Components/ArrowComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GSF/Character/GSFCharacter.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AGSFCamera::AGSFCamera()
{
	PrimaryActorTick.bCanEverTick = true;

	arrow = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow"));
	boom = CreateDefaultSubobject<USpringArmComponent>(TEXT("Boom"));
	camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	
	if(arrow)
		RootComponent = arrow;
	if(arrow && boom)
		boom->SetupAttachment(arrow);
	if(boom && camera)
		camera->SetupAttachment(boom);
}

void AGSFCamera::BeginPlay()
{
	Super::BeginPlay();
	
	controlledPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
}

void AGSFCamera::UpdateRotation(const float DeltaTime)
{
	if (!IsValid(arrow))return;
	if(bManualAim)return;

	// 処理落ち対策
	float rotateCorrection = 60.f / (1.0f / DeltaTime);

	// 角度計算
	FRotator rotation = arrow->GetRelativeRotation();
	// pitch制限
	rotation.Pitch = FMath::Clamp(rotation.Pitch + (inputValue.Y * pitch_sensitivity * rotateCorrection), current_pitch_min, current_pitch_max);
	rotation.Yaw += inputValue.X * yaw_sensitivity * rotateCorrection;
	// 距離補正
	float newLength = (-rotation.Pitch - current_pitch_min) / (current_pitch_max - current_pitch_min) * (current_boom_length_max - boom_length_min) + boom_length_min;
	boom->TargetArmLength = newLength;
	// 回転
	arrow->SetRelativeRotation(rotation);
}

void AGSFCamera::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(!IsValid(controlledPawn))return;
	SetActorLocation(controlledPawn->GetActorLocation());
	UpdateRotation(DeltaTime);
}

void AGSFCamera::InputAxis_Pitch(const float Value)
{
	bDeadZone = Value >= -deadZone && Value <= deadZone;
	inputValue.Y = Value;
}

void AGSFCamera::InputAxis_Yaw(const float Value)
{
	bDeadZone = Value >= -deadZone && Value <= deadZone;
	inputValue.X = Value;
}
