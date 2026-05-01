#pragma once
#include <functional>
#include <atomic>
#include <concepts>
#include <algorithm>
#include "../Core/CoreTypes.h"



//임의의 T Type의 instance가 uint32를 return하는 GetUUID 함수를 가지고 있는지 점검하는 type
// AddDynamic에 넘길 수 있는 T의 조건: GetUUID()가 uint32로 변환 가능한 값을 반환해야 함
template<class T>
concept HasGetUUID = requires(T v)
{
	{ v.GetUUID() } -> std::convertible_to<uint32>;
};

template<class... Args>
class TDelegate
{
public:
	// 구체적인 instance가 다를 수 있지만 function의 type이 void return type , Args... parameter인 함수를 모두 지칭하는 Type
	// Delegate가 가지는 ID - function pair
	using HandlerType = TPair< uint32, std::function<void(Args...)>>;
	using FunctionType = std::function<void(Args...) >;

	// instance와 상관없는 normal function
	void Add(const FunctionType& handler)
	{
		uint32 NewID = NextID.fetch_add(1);
		Handlers.push_back(HandlerType(NewID , handler));
	};


	//instance정보를 함께 담아가는 dynamic funciton
	template<HasGetUUID T>
	void AddDynamic(T* Instance, void (T::* Function)(Args...))
	{

		uint32 InstanceUUID = Instance->GetUUID();
		auto InstanceFunction = [Instance, Function](Args... args) {
			(Instance->*Function)(args...);
		};
		Handlers.push_back(HandlerType(InstanceUUID , InstanceFUnction));
	}

	void BroadCast(const Args&... args)
	{
		if (Handlers.empty())
		{
			return;
		}

		for (const auto& handler : Handlers)
		{
			if (handler.second)
			{
				handler.second(args...);
			}
		}

	};

	// instance UUID 기준 해당 instance handler를 제거하는 logic
	void RemoveDynamic(uint32 InstanceID)
	{
		Handlers.erase(std::remove_if(Handlers.begin(), Handlers.end(),
			[InstanceID](const HandlerType& handler) {
			return handler.first == InstanceID;
			}),
			Handlers.end()
		);
	}

private:

	// UUID와 NextID의 충돌 문제가 발생할 수 있음
	// Vector를 2개 놓던지, ID를 따로 관리할 필요가 있음
	TArray<HandlerType> Handlers;
	inline static std::atomic<uint32> NextID{ 0 };

};
