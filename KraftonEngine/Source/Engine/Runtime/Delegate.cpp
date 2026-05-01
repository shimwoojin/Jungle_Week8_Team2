#include <functional>
#include "../Core/CoreTypes.h"


template<class... Args>
class TDelegate
{
public:
	// 구체적인 instance가 다를 수 있지만 function의 type이 void return type , Args... parameter인 함수를 모두 지칭하는 Type
	using HandlerType = std::function<void(Args...)>;


	void Add(const HandlerType& handler)
	{


	}

	template<class T>
	void AddDynamic(T* Instance, void (T::* Function)(Args...))
	{
		auto InstanceFunction = [Instance, Function](Args... args) {
			
			(Instance->(*Function)(args...); 
				
		};
		Handlers.push_back(InstanceFunction);
	}

	void BroadCast(Args... args);

private:

	TArray<HandlerType> Handlers;



};

template<class ...Args>
void TDelegate<Args...>::BroadCast(Args ...args)
{
	for (const auto& handler : Handlers)
	{
		if (handler)
		{
			handler(args...);
		}
	}

}
