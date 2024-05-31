#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include <functional>
#include <vector>

#include "ActionType.h"

#include"ActionExecutor.generated.h"

ENUM_RANGE_BY_FIRST_AND_LAST(EAction, EAction::None, EAction::ElementsNum);

UCLASS()
class UMethodExecutor : public UActorComponent
{
	GENERATED_BODY()

	using MethodType = std::function<void()>;
	using AxisMethodType = std::function<void(float)>;
	using ActionMethodType = std::function<void(EAction, float)>;

private:
	
	TMap<EAction, TArray<MethodType>> Methods;
	
	TMap<EAction, TArray<AxisMethodType>> AxisMethods;

	TMap<EAction, TArray<ActionMethodType>> ActionMethods;

public:


	void AddMethod(const MethodType& Method, const EAction Action = EAction::None)
	{
		if(!Methods.Contains(Action))
		{
			Methods.Add(Action);
		}
		Methods[Action].Add(Method);
	}

	void ExecuteMethod(const EAction Action)
	{
		if(Methods.Contains(Action))
		{
			for (auto& Method : Methods[Action])
			{
				Method();
			}
		}
	}

	void AddAxisMethod(const AxisMethodType& Method, const EAction Action)
	{
		if(!AxisMethods.Contains(Action))
		{
			AxisMethods.Add(Action);
		}
		AxisMethods[Action].Add(Method);
	}

	void ExecuteAxisMethod(const EAction Action, const float Value)
	{
		if(AxisMethods.Contains(Action))
		{
			for (auto& Method : AxisMethods[Action])
			{
				Method(Value);
			}
		}
	}

	void AddActionMethod(const ActionMethodType& Method)
	{
		for(EAction aEnum : TEnumRange<EAction>())
		{
			TryAdd(Method, aEnum);
		}
	}

	void ExecuteActionMethod(const EAction Action, const float Reaction = 0.f)
	{
		if(ActionMethods.Contains(Action))
		{
			for (auto& Method : ActionMethods[Action])
			{
				Method(Action, Reaction);
			}
		}
	}

private:

	void TryAdd(const ActionMethodType& Method, const EAction Action)
	{
		if(!ActionMethods.Contains(Action))
		{
			ActionMethods.Add(Action);
		}
		ActionMethods[Action].Add(Method);
	}
	
};