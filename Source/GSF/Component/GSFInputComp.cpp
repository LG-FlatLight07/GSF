
#include "GSFInputComp.h"
#include "Components/InputComponent.h"
#include "GSF/Camera/GSFCamera.h"
#include "GSF/GameMode/GSFGameMode.h"
#include "Kismet/GameplayStatics.h"

UGSFInputComp::UGSFInputComp()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UGSFInputComp::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
		
	if(!character)return;

	// 二段入力
	DoubleInputCount(DeltaTime);

	// 先行入力
	BufferedInputCount(DeltaTime);

	// 長押し判定
	LongInputCheck(DeltaTime, waitForGraidInputTimeLimit, graidInputTimer, bPresseGraidKey);
}

void UGSFInputComp::SetupInputComp(UInputComponent* PlayerInputComponent)
{
	if (PlayerInputComponent)
	{
		PlayerInputComponent->BindAxis("MoveForward", this, &UGSFInputComp::InputAxis_MoveForward);
		PlayerInputComponent->BindAxis("MoveRight", this, &UGSFInputComp::InputAxis_MoveRight);

		PlayerInputComponent->BindAxis("Pitch", this, &UGSFInputComp::InputAxis_Pitch);
		PlayerInputComponent->BindAxis("Yaw", this, &UGSFInputComp::InputAxis_Yaw);
		
		PlayerInputComponent->BindAction("Attack", IE_Pressed,  this, &UGSFInputComp::InputAction_Attack);
		PlayerInputComponent->BindAction("ManualAim", IE_Pressed,  this, &UGSFInputComp::InputAction_ManualAim);
		PlayerInputComponent->BindAction("ManualAim", IE_Released,  this, &UGSFInputComp::InputAction_ManualAim_Released);
		PlayerInputComponent->BindAction("AirDash", IE_Pressed,  this, &UGSFInputComp::InputAction_AirDash);
		PlayerInputComponent->BindAction("AirDash", IE_Released, this, &UGSFInputComp::InputAction_AirDash_Released);
		PlayerInputComponent->BindAction("Bullet", IE_Pressed,  this, &UGSFInputComp::InputAction_Bullet);
		PlayerInputComponent->BindAction("Bullet", IE_Released, this, &UGSFInputComp::InputAction_Bullet_Released);
		PlayerInputComponent->BindAction("Concentration", IE_Pressed,  this, &UGSFInputComp::InputAction_Concentration);
		PlayerInputComponent->BindAction("Concentration", IE_Released, this, &UGSFInputComp::InputAction_Concentration_Released);
	}
}

void UGSFInputComp::BeginPlay()
{
	Super::BeginPlay();

	character = Cast<AGSFCharacter>(GetOwner());
}

void UGSFInputComp::BufferedInputCount(float DeltaTime)
{
	// アクションを待機中なら
	if (bWaitingAction)
	{
		// 待機時間が限界
		if (bufferedInputTimer >= timeToWait_BufferedInputExecution)
		{
			// 待機をやめる
			StopWaitForAction();
		}

		// 待機中のアクションを実行
		ExecutePendingActions();

		// 待機時間をカウント
		bufferedInputTimer += DeltaTime;
	}
}

void UGSFInputComp::ExecutePendingActions()
{
	// 何かのアクションを実行してたらやめる
	if (character->IsTakingAction())return;
	// 不正値を弾く
	if (WaitingAction == EInputActions::None)return;
	if (WaitingAction == EInputActions::ElementsNum)return;
	
	// Characterに通知
	character->TakeAction(WaitingAction);

	// 待機をやめる
	StopWaitForAction();
}

void UGSFInputComp::WaitForAction(EInputActions Action)
{
	// アクションを待機中に
	bWaitingAction = true;
	// 待機中のアクションを変更
	WaitingAction = Action;
	// 待機可能時間をリセット
	bufferedInputTimer = 0.f;
}

void UGSFInputComp::StopWaitForAction()
{
	// アクションの待機をやめる
	bWaitingAction = false;
	// 待機中のアクションを変更
	WaitingAction = EInputActions::None;
	// 待機可能時間をリセット
	bufferedInputTimer = 0.f;
}

bool UGSFInputComp::DoubleInputCheck(const EInputActions Action, const EInputActions ExAction = EInputActions::None)
{
	// 入力がある
    if (Action == EInputActions::None)return false;

	// 二回入力のフラグが立っている
    if (bWaitForDoubleInput)
    {
		// 一回目の入力と今回の入力が違う
		if (beforeInputAction != Action)
		{
			// 二回入力の猶予管理タイマーをリセット
			doubleInputTimer = 0.f;
			return false;
		}
	    
	    // 二回入力成功
    	if(ExAction == EInputActions::None)return false;
	    if (character->IsTakingAction())
	    {
    		WaitForAction(ExAction);
	    }
	    else
	    {
    		character->TakeAction(ExAction);
	    }
    	
	    // フラグ削除
	    bWaitForDoubleInput = false;
    	return true;
    }
    else
    {
	    // 二回入力のフラグを立てる
	    bWaitForDoubleInput = true;
	    // 二回入力の猶予管理タイマーをリセット
	    doubleInputTimer = 0.f;
    }
    // 前回の入力値を保存
    beforeInputAction = Action;
	return false;
}

void UGSFInputComp::DoubleInputCount(float DeltaTime)
{
	// 二回入力の猶予カウント
	if (bWaitForDoubleInput)
	{
		if (doubleInputTimer >= waitForDoubleInputTimeLimit)
		{
			bWaitForDoubleInput = false;
		}
		doubleInputTimer += DeltaTime;
	}
}

void UGSFInputComp::LongInputCheck(const float DeltaTime, const float Limit, float& Counter, bool& Action)
{
	if(!Action)
	{
		Counter = 0.f;
		return;
	}
	if(Counter >= Limit)
	{
		// 仮
		EventGraid();

		Counter = 0.f;
		Action = false;
		UE_LOG(LogTemp, Log, TEXT("Execute"));
		
		return;
	}
	UE_LOG(LogTemp, Log, TEXT("Count"));
	Counter += DeltaTime;
}

void UGSFInputComp::EventGraid()
{
	character->Graid_Implementation(bPresseGraidKey);
}

void UGSFInputComp::InputAxis_Pitch(const float Value)
{
	AGSFCamera* camera = Cast<AGSFGameMode>(UGameplayStatics::GetGameMode(GetWorld()))->camera;
	if(IsValid(camera))
		camera->InputAxis_Pitch(Value);
}

void UGSFInputComp::InputAxis_Yaw(const float Value)
{
	AGSFCamera* camera = Cast<AGSFGameMode>(UGameplayStatics::GetGameMode(GetWorld()))->camera;
	if(IsValid(camera))
		camera->InputAxis_Yaw(Value);
}

void UGSFInputComp::InputAxis_MoveForward(const float Value)
{
	if(!character)return;
	// キャラクターの移動メソッド
	character->MoveForward(Value);
}

void UGSFInputComp::InputAxis_MoveRight(const float Value)
{
	if(!character)return;
	// キャラクターの移動メソッド
	character->MoveRight(Value);
}

void UGSFInputComp::InputAction_Attack()
{
	if (character->IsTakingAction())
	{
		WaitForAction(EInputActions::Attack);
	}
	else
	{
		character->TakeAction(EInputActions::Attack);
	}
}

void UGSFInputComp::InputAction_ManualAim()
{
	if(!character)return;

	bPressedManualAimKey = true;
	character->ManualAim(bPressedManualAimKey);
}

void UGSFInputComp::InputAction_ManualAim_Released()
{
	bPressedManualAimKey = false;
	character->ManualAim(bPressedManualAimKey);
}

void UGSFInputComp::InputAction_AirDash()
{
	if(!character)return;

	// グライドの長押し判定
	bPresseGraidKey = true;
	
	// 二段入力に成功
	if(DoubleInputCheck(EInputActions::AirDash, EInputActions::Fly))
	{
		return;
	}

	// 行動中
	if (character->IsTakingAction())
	{
		// 待機
		WaitForAction(EInputActions::AirDash);
	}
	// 停止中
	else
	{
		// 実行
		character->TakeAction(EInputActions::AirDash);
	}
}

void UGSFInputComp::InputAction_AirDash_Released()
{
	bPresseGraidKey = false;
	character->Graid_Implementation(bPresseGraidKey);
}

void UGSFInputComp::InputAction_Bullet()
{
	bPresseBulletKey = true;
}

void UGSFInputComp::InputAction_Bullet_Released()
{
	bPresseBulletKey = false;
}

void UGSFInputComp::InputAction_Concentration()
{
	bPressedConcentrationKey = true;
	AGSFCamera* camera = Cast<AGSFGameMode>(UGameplayStatics::GetGameMode(GetWorld()))->c_camera;
	UGameplayStatics::GetPlayerController(GetWorld(),0)->SetViewTargetWithBlend(camera, 0.1f);
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 0.2f);
}

void UGSFInputComp::InputAction_Concentration_Released()
{
	bPressedConcentrationKey = false;
	AGSFCamera* camera = Cast<AGSFGameMode>(UGameplayStatics::GetGameMode(GetWorld()))->camera;
	UGameplayStatics::GetPlayerController(GetWorld(),0)->SetViewTargetWithBlend(camera, 0.1f);
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1.f);
}