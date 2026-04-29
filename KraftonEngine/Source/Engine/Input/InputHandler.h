#pragma once
#include "InputSystem.h"
#include "Core/CoreTypes.h"

#include <functional>
#include <memory>

enum class EInputKey {
	// Keyboard
	Q, W, E, R, T, Y, U, I, O, P,
	A, S, D, F, G, H, J, K, L,
	Z, X, C, V, B, N, M,
	Space, LShift, RShift, Tab, LCtrl, RCtrl, Backspace, Esc,
	Num0, Num1, Num2, Num3, Num4, Num5, Num6, Num7, Num8, Num9, Tilde,

	// Mouse
	MouseLeft, MouseRight, MouseWheel,

	// Guard
	None,
};

inline int ToVK(EInputKey Key)
{
	switch (Key)
	{
	case EInputKey::Q:          return 'Q';
	case EInputKey::W:          return 'W';
	case EInputKey::E:          return 'E';
	case EInputKey::R:          return 'R';
	case EInputKey::T:          return 'T';
	case EInputKey::Y:          return 'Y';
	case EInputKey::U:          return 'U';
	case EInputKey::I:          return 'I';
	case EInputKey::O:          return 'O';
	case EInputKey::P:          return 'P';
	case EInputKey::A:          return 'A';
	case EInputKey::S:          return 'S';
	case EInputKey::D:          return 'D';
	case EInputKey::F:          return 'F';
	case EInputKey::G:          return 'G';
	case EInputKey::H:          return 'H';
	case EInputKey::J:          return 'J';
	case EInputKey::K:          return 'K';
	case EInputKey::L:          return 'L';
	case EInputKey::Z:          return 'Z';
	case EInputKey::X:          return 'X';
	case EInputKey::C:          return 'C';
	case EInputKey::V:          return 'V';
	case EInputKey::B:          return 'B';
	case EInputKey::N:          return 'N';
	case EInputKey::M:          return 'M';
	case EInputKey::Space:      return VK_SPACE;
	case EInputKey::LShift:     return VK_LSHIFT;
	case EInputKey::RShift:     return VK_RSHIFT;
	case EInputKey::Tab:        return VK_TAB;
	case EInputKey::LCtrl:      return VK_LCONTROL;
	case EInputKey::RCtrl:      return VK_RCONTROL;
	case EInputKey::Backspace:  return VK_BACK;
	case EInputKey::Esc:        return VK_ESCAPE;
	case EInputKey::Num0:       return '0';
	case EInputKey::Num1:       return '1';
	case EInputKey::Num2:       return '2';
	case EInputKey::Num3:       return '3';
	case EInputKey::Num4:       return '4';
	case EInputKey::Num5:       return '5';
	case EInputKey::Num6:       return '6';
	case EInputKey::Num7:       return '7';
	case EInputKey::Num8:       return '8';
	case EInputKey::Num9:       return '9';
	case EInputKey::Tilde:      return VK_OEM_3;
	case EInputKey::MouseLeft:  return VK_LBUTTON;
	case EInputKey::MouseRight: return VK_RBUTTON;
	case EInputKey::MouseWheel: return 0;   // No VK; use InputSystem scroll API
	default:                    return 0;
	}
}

inline EInputKey FromVK(int VK)
{
	switch (VK)
	{
	case 'Q':          return EInputKey::Q;
	case 'W':          return EInputKey::W;
	case 'E':          return EInputKey::E;
	case 'R':          return EInputKey::R;
	case 'T':          return EInputKey::T;
	case 'Y':          return EInputKey::Y;
	case 'U':          return EInputKey::U;
	case 'I':          return EInputKey::I;
	case 'O':          return EInputKey::O;
	case 'P':          return EInputKey::P;

	case 'A':          return EInputKey::A;
	case 'S':          return EInputKey::S;
	case 'D':          return EInputKey::D;
	case 'F':          return EInputKey::F;
	case 'G':          return EInputKey::G;
	case 'H':          return EInputKey::H;
	case 'J':          return EInputKey::J;
	case 'K':          return EInputKey::K;
	case 'L':          return EInputKey::L;

	case 'Z':          return EInputKey::Z;
	case 'X':          return EInputKey::X;
	case 'C':          return EInputKey::C;
	case 'V':          return EInputKey::V;
	case 'B':          return EInputKey::B;
	case 'N':          return EInputKey::N;
	case 'M':          return EInputKey::M;

	case VK_SPACE:     return EInputKey::Space;
	case VK_LSHIFT:    return EInputKey::LShift;
	case VK_RSHIFT:    return EInputKey::RShift;
	case VK_TAB:       return EInputKey::Tab;
	case VK_LCONTROL:  return EInputKey::LCtrl;
	case VK_RCONTROL:  return EInputKey::RCtrl;
	case VK_BACK:      return EInputKey::Backspace;
	case VK_ESCAPE:    return EInputKey::Esc;

	case '0':          return EInputKey::Num0;
	case '1':          return EInputKey::Num1;
	case '2':          return EInputKey::Num2;
	case '3':          return EInputKey::Num3;
	case '4':          return EInputKey::Num4;
	case '5':          return EInputKey::Num5;
	case '6':          return EInputKey::Num6;
	case '7':          return EInputKey::Num7;
	case '8':          return EInputKey::Num8;
	case '9':          return EInputKey::Num9;

	case VK_OEM_3:     return EInputKey::Tilde;

	case VK_LBUTTON:   return EInputKey::MouseLeft;
	case VK_RBUTTON:   return EInputKey::MouseRight;

		// Mouse wheel has no real VK equivalent
		// Handle separately through WM_MOUSEWHEEL or your input system
	default:           return EInputKey::None; // or Invalid if you have one
	}
}

enum class EInputEvent {
	Pressed,    // First frame down
	Down,       // Held
	Released,   // First frame up
	Idle,

	// Mouse-specific
	Moved,
	Drag,
	Click,      // Down + Released within threshold
};

struct FDeferredCommand {
	void Tick(float DeltaTime) {
		if (RemainingTimer > 0) {
			RemainingTimer -= DeltaTime;

			if (RemainingTimer <= 0 && Command) {
				Command();
			}
		}
	}

	float Timer;
	float RemainingTimer;

	// Function to execute
	std::function<void()> Command;
};

struct FInputBinding {
	EInputKey				Key;
	EInputEvent				Event;
	std::function<void()>	Action;
};

struct FInputBindingHasher {
	static uint64_t Hash(EInputKey Key, EInputEvent Event) {
		return (static_cast<uint64_t>(Key) << 32)
			| static_cast<uint64_t>(Event);
	}

	uint64_t operator()(uint64_t Hash) const {
		return Hash;
	}
};

struct FTrieNode {
	TMap<uint64_t, std::unique_ptr<FTrieNode>> Children;
	std::function<void()> Action;   // Set only on terminal nodes

	bool IsTerminal() const { return Action != nullptr; }
};

class FBindingTrie {
public:
	void Tick(float DeltaTime) {
		Elapsed += DeltaTime;
		if (Elapsed >= LifeSpan) Reset();
	}

	void Insert(const TArray<FInputBinding>& Sequence, std::function<void()> Action) {
		FTrieNode* Current = &Root;
		for (FInputBinding Binding : Sequence) {
			auto Key = FInputBindingHasher::Hash(Binding.Key, Binding.Event);
			auto& Child = Current->Children[Key];
			if (!Child) Child = std::make_unique<FTrieNode>();
			Current = Child.get();
		}
		Current->Action = std::move(Action);
	}

	bool Advance(EInputKey InputKey, EInputEvent InputEvent) {
		if (!Cursor) Cursor = &Root;

		auto Key = FInputBindingHasher::Hash(InputKey, InputEvent);
		auto It = Cursor->Children.find(Key);
		if (It == Cursor->Children.end()) {
			Reset();        // No match
			return false;
		}

		Cursor = It->second.get();

		if (Cursor->IsTerminal()) {
			Cursor->Action();
			Reset();        // Consume the combo
			return true;
		}

		return false;       // Mid-sequence
	}

	void Reset() { Cursor = &Root; Elapsed = 0.f; }

private:
	FTrieNode  Root;
	FTrieNode* Cursor = &Root;  // Tracks where we are mid-combo

	float	   LifeSpan = 0.5f;
	float	   Elapsed  = 0.f;
};

// Binds raw input with a command/combo
class FInputHandler {
public:
	void Tick(float DeltaTime) {
		Trie.Tick(DeltaTime);
	}

	// Register a key / key combo to Trie
	void RegisterBinding(const TArray<FInputBinding>& Bindings, std::function<void()> Action) {
		Trie.Insert(Bindings, Action);
	}

	void QueryKeyEvent()
	{
		InputSystem& IS = InputSystem::Get();

		for (uint16 i = 0; i < static_cast<uint16>(EInputKey::None); i++)
		{
			EInputKey Key = static_cast<EInputKey>(i);

			if (Key == EInputKey::MouseWheel)
			{
				if (IS.GetScrollDelta() != 0)
					Dispatch(Key, EInputEvent::Pressed);
				continue;
			}

			int VK = ToVK(Key);
			if (VK == 0) continue;

			if (IS.GetKeyDown(VK))		Dispatch(Key, EInputEvent::Pressed);
			else if (IS.GetKeyUp(VK))   Dispatch(Key, EInputEvent::Released);
			else if (IS.GetKey(VK))     Dispatch(Key, EInputEvent::Down);
		}
	}

	void Dispatch(EInputKey Key, EInputEvent Event)
	{
		uint64_t Hash = FInputBindingHasher::Hash(Key, Event);
		auto It = Bindings.find(Hash);
		if (It != Bindings.end())
			for (auto& Action : It->second)
				Action();

		if (Event == EInputEvent::Pressed)
			Trie.Advance(Key, Event);
	}

private:
	TMap<uint64_t, TArray<std::function<void()>>> Bindings;  // single-key bindings
	FBindingTrie Trie;

};