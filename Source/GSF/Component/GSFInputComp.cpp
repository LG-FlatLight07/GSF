
#include "GSFInputComp.h"

#include "Components/InputComponent.h"

UGSFInputComp::UGSFInputComp()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UGSFInputComp::BeginPlay()
{
	Super::BeginPlay();

	character = Cast<AGSFCharacter>(GetOwner());
	if(IsValid(character))
	{
		InputBind(character->InputComponent);
	}
}

void UGSFInputComp::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// 二段入力処理
	DoubleInputCount(DeltaTime);
	// 先行入力処理
	BufferedInputCount(DeltaTime);
	// 長押し入力処理
	LongInputCount(DeltaTime);
}

void UGSFInputComp::InputBind(UInputComponent* InputComponent)
{
	if(IsValid(InputComponent))
	{
		InputComponent->BindAxis("MoveForward", this, &UGSFInputComp::Forward);
		InputComponent->BindAxis("MoveRight", this, &UGSFInputComp::Right);
		
		InputComponent->BindAxis("Pitch", this, &UGSFInputComp::Pitch);
		InputComponent->BindAxis("Yaw", this, &UGSFInputComp::Yaw);
		
		InputComponent->BindAction("A", IE_Pressed, this, &UGSFInputComp::A);
		InputComponent->BindAction("B", IE_Pressed, this, &UGSFInputComp::B);
		InputComponent->BindAction("X", IE_Pressed, this, &UGSFInputComp::X);
		InputComponent->BindAction("Y", IE_Pressed, this, &UGSFInputComp::Y);
		
		InputComponent->BindAction("L1", IE_Pressed, this, &UGSFInputComp::L1);
		InputComponent->BindAction("L1", IE_Released, this, &UGSFInputComp::L1_Release);
		InputComponent->BindAction("R1", IE_Pressed, this, &UGSFInputComp::R1);
		InputComponent->BindAction("R1", IE_Released, this, &UGSFInputComp::R1_Release);

		InputComponent->BindAction("L2", IE_Pressed, this, &UGSFInputComp::L2);
		InputComponent->BindAction("L2", IE_Released, this, &UGSFInputComp::L2_Release);
		InputComponent->BindAction("R2", IE_Pressed, this, &UGSFInputComp::R2);
		InputComponent->BindAction("R2", IE_Released, this, &UGSFInputComp::R2_Release);
	}
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
	if (WaitingAction.Key == EInput::None)return;
	if (WaitingAction.Key == EInput::ElementsNum)return;
	
	// Characterに通知
	if(onInputEvent.IsBound())
	{
		onInputEvent.Execute(WaitingAction.Key, WaitingAction.Value);
	}

	// 待機をやめる
	StopWaitForAction();
}

void UGSFInputComp::WaitForAction(const EInput Input, const EInputType Type)
{
	// アクションを待機中に
	bWaitingAction = true;
	// 待機中のアクションを変更
	WaitingAction.Key = Input;
	WaitingAction.Value = Type;
	// 待機可能時間をリセット
	bufferedInputTimer = 0.f;
}

void UGSFInputComp::StopWaitForAction()
{
	// アクションの待機をやめる
	bWaitingAction = false;
	// 待機中のアクションを変更
	WaitingAction.Key = EInput::None;
	WaitingAction.Value = EInputType::None;
	// 待機可能時間をリセット
	bufferedInputTimer = 0.f;
}

bool UGSFInputComp::DoubleInputCheck(const EInput Action)
{
	if(Action == EInput::None)return false;
	if(Action == EInput::ElementsNum)return false;
	
	if(firstInputAction == Action && bWaitForDoubleInput)
	{
		if (character->IsTakingAction())
		{
			if(character->IsCantBufferedInput())return false;
			WaitForAction(Action, EInputType::Double);
		}
		else
		{
			onInputEvent.Execute(Action, EInputType::Double);
		}
		firstInputAction = EInput::None;
		bWaitForDoubleInput = false;
		return true;
	}
	firstInputAction = Action;
	bWaitForDoubleInput = true;
	doubleInputTimer = 0.f;
	return false;
}

void UGSFInputComp::DoubleInputCount(const float DeltaTime)
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

void UGSFInputComp::LongInputCount(const float DeltaTime)
{
	for(const auto& Key : pressedFlg)
	{
		if(pressedFlg[Key.Key] == false)continue;
		
		if(pressedTimer[Key.Key] >= longInputTime)
		{
			if (character->IsTakingAction())
			{
				if(character->IsCantBufferedInput())return;
				WaitForAction(Key.Key, EInputType::Long);
			}
			else
			{
				onInputEvent.Execute(Key.Key, EInputType::Long);
			}
			pressedFlg[Key.Key] = false;
			pressedTimer[Key.Key] = 0.f;
			continue;
		}
		pressedTimer[Key.Key] += DeltaTime;
	}
}

void UGSFInputComp::Input(const EInput Action, const EInputType Type)
{
	if(DoubleInputCheck(Action))return;
	
	if (character->IsTakingAction())
	{
		if(character->IsCantBufferedInput())return;
		WaitForAction(Action, Type);
	}
	else if(onInputEvent.IsBound())
	{
		onInputEvent.Execute(Action, Type);
	}
}

void UGSFInputComp::LongInput(const EInput Action)
{
	// 二段入力検知
	if(DoubleInputCheck(Action))return;

	// 長押し待機開始
	if(pressedFlg.Contains(Action))
	{
		pressedFlg[Action] = true;
		if(pressedTimer.Contains(Action))
		{
			pressedTimer[Action] = 0.f;
		}
		else
		{
			pressedTimer.Add(Action, 0.f);
		}
	}
	else
	{
		pressedFlg.Add(Action, true);
		if(pressedTimer.Contains(Action))
		{
			pressedTimer[Action] = 0.f;
		}
		else
		{
			pressedTimer.Add(Action, 0.f);
		}
	}

	// 単押し
	if (character->IsTakingAction())
	{
		if(character->IsCantBufferedInput())return;
		WaitForAction(Action, EInputType::Single);
	}
	else if(onInputEvent.IsBound())
	{
		onInputEvent.Execute(Action, EInputType::Single);
	}
}

void UGSFInputComp::Forward(const float Value)
{
	input.Y = Value;
	if(onAxisInputEvent.IsBound())
	{
		onAxisInputEvent.Execute(EInput::Forward, EInputType::Single, Value);
	}
}

void UGSFInputComp::Right(const float Value)
{
	input.X = Value;
	if(onAxisInputEvent.IsBound())
	{
		onAxisInputEvent.Execute(EInput::Right, EInputType::Single, Value);
	}
}

void UGSFInputComp::Pitch(const float Value)
{
	if(onAxisInputEvent.IsBound())
	{
		onAxisInputEvent.Execute(EInput::Pitch, EInputType::Single, Value);
	}
}

void UGSFInputComp::Yaw(const float Value)
{
	if(onAxisInputEvent.IsBound())
	{
		onAxisInputEvent.Execute(EInput::Yaw, EInputType::Single, Value);
	}
}

void UGSFInputComp::A()
{
	Input(EInput::A, EInputType::Single);
}

void UGSFInputComp::B()
{
	Input(EInput::B, EInputType::Single);
}

void UGSFInputComp::X()
{
	Input(EInput::X, EInputType::Single);
}

void UGSFInputComp::Y()
{
	Input(EInput::Y, EInputType::Single);
}

void UGSFInputComp::L1()
{
	LongInput(EInput::L1);
}

void UGSFInputComp::L1_Release()
{
	pressedFlg[EInput::L1] = false;
	pressedTimer[EInput::L1] = 0.f;

	if(onInputEvent.IsBound())
	{
		onInputEvent.Execute(EInput::L1, EInputType::Release);
	}
}

void UGSFInputComp::L2()
{
	LongInput(EInput::L2);
}

void UGSFInputComp::L2_Release()
{
	pressedFlg[EInput::L2] = false;
	pressedTimer[EInput::L2] = 0.f;
	
	if(onInputEvent.IsBound())
	{
		onInputEvent.Execute(EInput::L2, EInputType::Release);
	}
}

void UGSFInputComp::R1()
{
	LongInput(EInput::R1);
}

void UGSFInputComp::R1_Release()
{
	pressedFlg[EInput::R1] = false;
	pressedTimer[EInput::R1] = 0.f;
	
	if(onInputEvent.IsBound())
	{
		onInputEvent.Execute(EInput::R1, EInputType::Release);
	}
}

void UGSFInputComp::R2()
{
	LongInput(EInput::R2);
}

void UGSFInputComp::R2_Release()
{
	pressedFlg[EInput::R2] = false;
	pressedTimer[EInput::R2] = 0.f;
	
	if(onInputEvent.IsBound())
	{
		onInputEvent.Execute(EInput::R2, EInputType::Release);
	}
}

