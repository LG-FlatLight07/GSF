// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GSFCamera.generated.h"

class UArrowComponent;
class UCameraComponent;
class USpringArmComponent;

UCLASS()
class GSF_API AGSFCamera : public AActor
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UArrowComponent* arrow;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USpringArmComponent* boom;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCameraComponent* camera;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	APawn* controlledPawn;

	// デッドゾーン
	UPROPERTY(EditAnywhere)
	float deadZone = 0.05f;
	UPROPERTY(EditAnywhere)
	float pitch_sensitivity = 2.f;
	UPROPERTY(EditAnywhere)
	float yaw_sensitivity = 2.f;

	UPROPERTY(EditAnywhere)
	float pitch_min = -40.f;
	UPROPERTY(EditAnywhere)
	float current_pitch_min = -40.f;
	UPROPERTY(EditAnywhere)
	float dive_pitch_min = -40.f;
	
	UPROPERTY(EditAnywhere)
	float pitch_max = 65.f;
	UPROPERTY(EditAnywhere)
	float current_pitch_max = 65.f;
	UPROPERTY(EditAnywhere)
	float dive_pitch_max = 30.f;
	
	UPROPERTY(EditAnywhere)
	float boom_length_min = 180.f;

	UPROPERTY(EditAnywhere)
	float current_boom_length_max = 500.f;
	UPROPERTY(EditAnywhere)
	float boom_length_max = 500.f;
	UPROPERTY(EditAnywhere)
	float dive_boom_length_max = 400.f;
	UPROPERTY(EditAnywhere)
	float graid_boom_length_max = 520.f;
	UPROPERTY(EditAnywhere)
	float fly_boom_length_max = 480.f;
	
private:
	// デッドゾーン
	bool bDeadZone;
	// マニュアルエイム
	bool bManualAim;
	// 入力
	FVector2D inputValue;
	
public:
	AGSFCamera();
	virtual void Tick(float DeltaTime) override;

	void SetDiveBoomLength(){current_boom_length_max = dive_boom_length_max;}
	void SetGraidBoomLength(){current_boom_length_max = graid_boom_length_max;}
	void SetFlyBoomLength(){current_boom_length_max = fly_boom_length_max;}
	void ResetBoomLength(){current_boom_length_max = boom_length_max;}
	void SetDivePitch(){current_pitch_min = dive_pitch_min; current_pitch_max = dive_pitch_max;}
	void ResetPitch(){current_pitch_min = pitch_min; current_pitch_max = pitch_max;}
	
	void SetManualAim(const bool aim){bManualAim = aim;};
	
	/*------------------------------------------------------------------------------------------------------------------------------
	* 入力制御
	-------------------------------------------------------------------------------------------------------------------------------*/
	void InputAxis_Pitch(const float Value);
	void InputAxis_Yaw(const float Value);

protected:
	virtual void BeginPlay() override;

private:
	void UpdateRotation(const float DeltaTime);
	
};
