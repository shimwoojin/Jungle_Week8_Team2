#include "Engine/UI/Game/GameUiSystem.h"

#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>

#include "Engine/UI/Rml/RmlD3D11RenderInterface.h"
#include "Engine/UI/Rml/RmlWin32SystemInterface.h"
#include "Engine/Runtime/WindowsWindow.h"
#include "Render/Pipeline/Renderer.h"
#include "Viewport/GameViewportClient.h"

#include <algorithm>
#include <utility>

#include "Engine/UI/Rml/RmlUiConfig.h"
#if WITH_RMLUI
#include "Engine/UI/Rml/RmlUiWindowsMacroGuard.h"
#include <RmlUi/Core.h>
#include <RmlUi/Core/Context.h>
#include <RmlUi/Core/Element.h>
#include <RmlUi/Core/ElementDocument.h>
#include <RmlUi/Core/Event.h>
#include <RmlUi/Core/EventListener.h>
#include <RmlUi/Core/Input.h>
#endif

namespace
{
int GetSignedMouseX(LPARAM Param)
	{
		return static_cast<int>(static_cast<short>(LOWORD(Param)));
	}

	int GetSignedMouseY(LPARAM Param)
	{
		return static_cast<int>(static_cast<short>(HIWORD(Param)));
	}

	int GetSignedWheelDelta(WPARAM Param)
	{
		return static_cast<int>(static_cast<short>(HIWORD(Param)));
	}

int ToRmlMouseButton(UINT Msg)
	{
		switch (Msg)
		{
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
			return 0;
		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP:
			return 1;
		case WM_MBUTTONDOWN:
		case WM_MBUTTONUP:
			return 2;
		default:
			return 0;
		}
	}

#if WITH_RMLUI
	int GetRmlModifierState()
	{
		int Modifiers = 0;
		if (::GetKeyState(VK_CONTROL) & 0x8000) Modifiers |= Rml::Input::KM_CTRL;
		if (::GetKeyState(VK_SHIFT) & 0x8000) Modifiers |= Rml::Input::KM_SHIFT;
		if (::GetKeyState(VK_MENU) & 0x8000) Modifiers |= Rml::Input::KM_ALT;
		if (::GetKeyState(VK_CAPITAL) & 0x0001) Modifiers |= Rml::Input::KM_CAPSLOCK;
		if (::GetKeyState(VK_NUMLOCK) & 0x0001) Modifiers |= Rml::Input::KM_NUMLOCK;
		if (::GetKeyState(VK_SCROLL) & 0x0001) Modifiers |= Rml::Input::KM_SCROLLLOCK;
		return Modifiers;
	}

	Rml::Input::KeyIdentifier ToRmlKey(int VirtualKey)
	{
		if (VirtualKey >= 'A' && VirtualKey <= 'Z')
		{
			return static_cast<Rml::Input::KeyIdentifier>(Rml::Input::KI_A + (VirtualKey - 'A'));
		}
		if (VirtualKey >= '0' && VirtualKey <= '9')
		{
			return static_cast<Rml::Input::KeyIdentifier>(Rml::Input::KI_0 + (VirtualKey - '0'));
		}
		if (VirtualKey >= VK_F1 && VirtualKey <= VK_F12)
		{
			return static_cast<Rml::Input::KeyIdentifier>(Rml::Input::KI_F1 + (VirtualKey - VK_F1));
		}
		if (VirtualKey >= VK_NUMPAD0 && VirtualKey <= VK_NUMPAD9)
		{
			return static_cast<Rml::Input::KeyIdentifier>(Rml::Input::KI_NUMPAD0 + (VirtualKey - VK_NUMPAD0));
		}

		switch (VirtualKey)
		{
		case VK_SPACE: return Rml::Input::KI_SPACE;
		case VK_BACK: return Rml::Input::KI_BACK;
		case VK_TAB: return Rml::Input::KI_TAB;
		case VK_RETURN: return Rml::Input::KI_RETURN;
		case VK_ESCAPE: return Rml::Input::KI_ESCAPE;
		case VK_PRIOR: return Rml::Input::KI_PRIOR;
		case VK_NEXT: return Rml::Input::KI_NEXT;
		case VK_END: return Rml::Input::KI_END;
		case VK_HOME: return Rml::Input::KI_HOME;
		case VK_LEFT: return Rml::Input::KI_LEFT;
		case VK_UP: return Rml::Input::KI_UP;
		case VK_RIGHT: return Rml::Input::KI_RIGHT;
		case VK_DOWN: return Rml::Input::KI_DOWN;
		case VK_INSERT: return Rml::Input::KI_INSERT;
		case VK_DELETE: return Rml::Input::KI_DELETE;
		case VK_SHIFT: return Rml::Input::KI_LSHIFT;
		case VK_LSHIFT: return Rml::Input::KI_LSHIFT;
		case VK_RSHIFT: return Rml::Input::KI_RSHIFT;
		case VK_CONTROL: return Rml::Input::KI_LCONTROL;
		case VK_LCONTROL: return Rml::Input::KI_LCONTROL;
		case VK_RCONTROL: return Rml::Input::KI_RCONTROL;
		case VK_MENU: return Rml::Input::KI_LMENU;
		case VK_LMENU: return Rml::Input::KI_LMENU;
		case VK_RMENU: return Rml::Input::KI_RMENU;
		case VK_OEM_PLUS: return Rml::Input::KI_OEM_PLUS;
		case VK_OEM_COMMA: return Rml::Input::KI_OEM_COMMA;
		case VK_OEM_MINUS: return Rml::Input::KI_OEM_MINUS;
		case VK_OEM_PERIOD: return Rml::Input::KI_OEM_PERIOD;
		case VK_OEM_1: return Rml::Input::KI_OEM_1;
		case VK_OEM_2: return Rml::Input::KI_OEM_2;
		case VK_OEM_3: return Rml::Input::KI_OEM_3;
		case VK_OEM_4: return Rml::Input::KI_OEM_4;
		case VK_OEM_5: return Rml::Input::KI_OEM_5;
		case VK_OEM_6: return Rml::Input::KI_OEM_6;
		case VK_OEM_7: return Rml::Input::KI_OEM_7;
		default: break;
		}
		return Rml::Input::KI_UNKNOWN;
	}
#endif
}

#if WITH_RMLUI
class FGameUiEventListener final : public Rml::EventListener
{
public:
	explicit FGameUiEventListener(FGameUiSystem* InOwner)
		: Owner(InOwner)
	{
	}

	void ProcessEvent(Rml::Event& Event) override
	{
		if (!Owner)
		{
			return;
		}

		Rml::Element* Element = Event.GetCurrentElement();
		if (!Element)
		{
			Element = Event.GetTargetElement();
		}
		if (!Element)
		{
			return;
		}

		Owner->HandleClick(Element->GetId());
	}

private:
	FGameUiSystem* Owner = nullptr;
};
#else
// WITH_RMLUI=0 빌드에서도 FGameUiSystem.h의
// std::unique_ptr<FGameUiEventListener>가 안전하게 소멸될 수 있도록
// 같은 번역 단위에서 완전한 타입을 제공한다.
class FGameUiEventListener final
{
public:
	explicit FGameUiEventListener(FGameUiSystem*) {}
};
#endif

FGameUiSystem::FGameUiSystem() = default;
FGameUiSystem::~FGameUiSystem()
{
	Shutdown();
}

bool FGameUiSystem::Initialize(FWindowsWindow* Window, FRenderer& Renderer, UGameViewportClient* InViewportClient)
{
	Shutdown();

	OwnerWindow = Window;
	ViewportClient = InViewportClient;
	if (!OwnerWindow || !ViewportClient)
	{
		return false;
	}

	PresentationRect = ViewportClient->GetPresentationRect();
	if (!PresentationRect.IsValid())
	{
		PresentationRect = FViewportPresentationRect(
			0.0f,
			0.0f,
			OwnerWindow->GetWidth(),
			OwnerWindow->GetHeight());
	}

#if WITH_RMLUI
	RenderInterface = std::make_unique<FRmlD3D11RenderInterface>();
	if (!RenderInterface->Initialize(
		Renderer.GetFD3DDevice().GetDevice(),
		Renderer.GetFD3DDevice().GetDeviceContext()))
	{
		Shutdown();
		return false;
	}
	RenderInterface->SetPresentationRect(PresentationRect);
	RenderInterface->SetRenderTargetSize(OwnerWindow->GetWidth(), OwnerWindow->GetHeight());

	SystemInterface = std::make_unique<FRmlWin32SystemInterface>();
	Rml::SetRenderInterface(RenderInterface.get());
	Rml::SetSystemInterface(SystemInterface.get());

	if (!Rml::Initialise())
	{
		Shutdown();
		return false;
	}

	Context = Rml::CreateContext(
		"GameUI",
		Rml::Vector2i(
			static_cast<int>(PresentationRect.Width),
			static_cast<int>(PresentationRect.Height)));
	if (!Context)
	{
		Rml::Shutdown();
		Shutdown();
		return false;
	}
	Context->EnableMouseCursor(true);

	Rml::LoadFontFace("C:/Windows/Fonts/malgun.ttf");
	LoadDocuments();

	bAvailable = Context != nullptr;
#else
	(void)Renderer;
	bAvailable = false;
#endif

	bInitialized = true;
	return true;
}

void FGameUiSystem::Shutdown()
{
#if WITH_RMLUI
	UnbindPauseMenuEvents();

	HudDocument = nullptr;
	PauseMenuDocument = nullptr;

	if (Context)
	{
		const Rml::String ContextName = Context->GetName();
		Rml::RemoveContext(ContextName);
		Context = nullptr;
	}

	EventListener.reset();

	if (bAvailable)
	{
		Rml::Shutdown();
	}

	if (RenderInterface)
	{
		RenderInterface->Shutdown();
	}
#endif

	RenderInterface.reset();
	SystemInterface.reset();

	OwnerWindow = nullptr;
	ViewportClient = nullptr;

	bInitialized = false;
	bAvailable = false;
	bPauseMenuVisible = false;
	bShowingOptions = false;
}

void FGameUiSystem::SetCallbacks(FGameUiCallbacks InCallbacks)
{
	Callbacks = std::move(InCallbacks);
	RefreshOptionLabels();
}

void FGameUiSystem::SetPresentationRect(const FViewportPresentationRect& InRect)
{
	if (!InRect.IsValid())
	{
		return;
	}
	PresentationRect = InRect;
	SyncContextDimensions();
}

void FGameUiSystem::SyncContextDimensions()
{
#if WITH_RMLUI
	if (Context)
	{
		Context->SetDimensions(Rml::Vector2i(
			static_cast<int>(PresentationRect.Width),
			static_cast<int>(PresentationRect.Height)));
	}
	if (RenderInterface)
	{
		RenderInterface->SetPresentationRect(PresentationRect);
		if (OwnerWindow)
		{
			RenderInterface->SetRenderTargetSize(OwnerWindow->GetWidth(), OwnerWindow->GetHeight());
		}
	}
#endif
}

void FGameUiSystem::Update(float DeltaTime)
{
	(void)DeltaTime;
	if (!bInitialized)
	{
		return;
	}

	if (ViewportClient && ViewportClient->HasValidPresentationRect())
	{
		SetPresentationRect(ViewportClient->GetPresentationRect());
	}

#if WITH_RMLUI
	RefreshOptionLabels();
	if (Context)
	{
		Context->Update();
	}
#endif
}

void FGameUiSystem::Render()
{
#if WITH_RMLUI
	if (!bInitialized || !Context || !RenderInterface)
	{
		return;
	}

	SyncContextDimensions();
	Context->Render();
#endif
}

void FGameUiSystem::SetPauseMenuVisible(bool bVisible)
{
	bPauseMenuVisible = bVisible;
	if (!bVisible)
	{
		bShowingOptions = false;
	}

#if WITH_RMLUI
	if (!PauseMenuDocument)
	{
		return;
	}

	if (bVisible)
	{
		PauseMenuDocument->Show();
		SetOptionsVisible(bShowingOptions);
	}
	else
	{
		PauseMenuDocument->Hide();
	}
#endif
}

bool FGameUiSystem::LoadDocuments()
{
#if WITH_RMLUI
	if (!Context)
	{
		return false;
	}

	HudDocument = Context->LoadDocument("Asset/UI/HUD.rml");
	if (HudDocument)
	{
		HudDocument->Show();
	}

	PauseMenuDocument = Context->LoadDocument("Asset/UI/PauseMenu.rml");
	if (PauseMenuDocument)
	{
		PauseMenuDocument->Hide();
	}

	BindPauseMenuEvents();
	RefreshOptionLabels();
	return HudDocument != nullptr || PauseMenuDocument != nullptr;
#else
	return false;
#endif
}

void FGameUiSystem::BindPauseMenuEvents()
{
#if WITH_RMLUI
	if (!PauseMenuDocument)
	{
		return;
	}

	if (!EventListener)
	{
		EventListener = std::make_unique<FGameUiEventListener>(this);
	}

	auto BindClick = [&](const char* ElementId)
	{
		if (Rml::Element* Element = PauseMenuDocument->GetElementById(ElementId))
		{
			Element->AddEventListener("click", EventListener.get(), false);
		}
	};

	BindClick("continue");
	BindClick("options");
	BindClick("restart");
	BindClick("exit");
	BindClick("back");
	BindClick("toggle-fullscreen");
	BindClick("toggle-fxaa");
#endif
}

void FGameUiSystem::HandleClick(const FString& ElementId)
{
	if (ElementId == "continue")
	{
		if (Callbacks.OnContinue)
		{
			Callbacks.OnContinue();
		}
		return;
	}
	if (ElementId == "options")
	{
		SetOptionsVisible(true);
		return;
	}
	if (ElementId == "restart")
	{
		if (Callbacks.OnRestart)
		{
			Callbacks.OnRestart();
		}
		return;
	}
	if (ElementId == "exit")
	{
		if (Callbacks.OnExit)
		{
			Callbacks.OnExit();
		}
		return;
	}
	if (ElementId == "back")
	{
		SetOptionsVisible(false);
		return;
	}
	if (ElementId == "toggle-fullscreen")
	{
		if (Callbacks.OnToggleFullscreen)
		{
			Callbacks.OnToggleFullscreen();
		}
		RefreshOptionLabels();
		return;
	}
	if (ElementId == "toggle-fxaa")
	{
		bool bNext = true;
		if (Callbacks.IsFxaaEnabled)
		{
			bNext = !Callbacks.IsFxaaEnabled();
		}
		if (Callbacks.OnFxaaChanged)
		{
			Callbacks.OnFxaaChanged(bNext);
		}
		RefreshOptionLabels();
		return;
	}
}

void FGameUiSystem::SetOptionsVisible(bool bVisible)
{
	bShowingOptions = bVisible;
	SetElementDisplay("main-panel", !bShowingOptions);
	SetElementDisplay("options-panel", bShowingOptions);
	RefreshOptionLabels();
}

void FGameUiSystem::RefreshOptionLabels()
{
#if WITH_RMLUI
	if (!PauseMenuDocument)
	{
		return;
	}

	const bool bFullscreen = Callbacks.IsFullscreen ? Callbacks.IsFullscreen() : false;
	SetElementText("toggle-fullscreen", bFullscreen ? "전체화면 해제" : "전체화면");

	const bool bFxaa = Callbacks.IsFxaaEnabled ? Callbacks.IsFxaaEnabled() : false;
	SetElementText("toggle-fxaa", bFxaa ? "FXAA: 켜짐" : "FXAA: 꺼짐");
#endif
}

void FGameUiSystem::SetElementDisplay(const char* ElementId, bool bVisible)
{
#if WITH_RMLUI
	if (!PauseMenuDocument || !ElementId)
	{
		return;
	}
	if (Rml::Element* Element = PauseMenuDocument->GetElementById(ElementId))
	{
		Element->SetProperty("display", bVisible ? "block" : "none");
	}
#else
	(void)ElementId;
	(void)bVisible;
#endif
}

void FGameUiSystem::SetElementText(const char* ElementId, const char* Text)
{
#if WITH_RMLUI
	if (!PauseMenuDocument || !ElementId || !Text)
	{
		return;
	}
	if (Rml::Element* Element = PauseMenuDocument->GetElementById(ElementId))
	{
		Element->SetInnerRML(Text);
	}
#else
	(void)ElementId;
	(void)Text;
#endif
}

bool FGameUiSystem::ProcessWin32Message(void* hWnd, uint32 Msg, std::uintptr_t wParam, std::intptr_t lParam)
{
	if (!bInitialized || !bAvailable)
	{
		return false;
	}

	HWND NativeHWnd = static_cast<HWND>(hWnd);
	WPARAM NativeWParam = static_cast<WPARAM>(wParam);
	LPARAM NativeLParam = static_cast<LPARAM>(lParam);

	switch (Msg)
	{
	case WM_MOUSEMOVE:
		return ProcessMouseMove(static_cast<float>(GetSignedMouseX(NativeLParam)), static_cast<float>(GetSignedMouseY(NativeLParam)));
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_MBUTTONDOWN:
		return ProcessMouseButtonDown(ToRmlMouseButton(Msg), static_cast<float>(GetSignedMouseX(NativeLParam)), static_cast<float>(GetSignedMouseY(NativeLParam)));
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MBUTTONUP:
		return ProcessMouseButtonUp(ToRmlMouseButton(Msg), static_cast<float>(GetSignedMouseX(NativeLParam)), static_cast<float>(GetSignedMouseY(NativeLParam)));
	case WM_MOUSEWHEEL:
	{
		POINT Point = { GetSignedMouseX(NativeLParam), GetSignedMouseY(NativeLParam) };
		if (NativeHWnd)
		{
			::ScreenToClient(NativeHWnd, &Point);
		}
		const float WheelDelta = static_cast<float>(GetSignedWheelDelta(NativeWParam)) / static_cast<float>(WHEEL_DELTA);
		return ProcessMouseWheel(-WheelDelta, static_cast<float>(Point.x), static_cast<float>(Point.y));
	}
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		return ProcessKeyDown(static_cast<int>(NativeWParam));
	case WM_KEYUP:
	case WM_SYSKEYUP:
		return ProcessKeyUp(static_cast<int>(NativeWParam));
	case WM_CHAR:
		return ProcessTextInput(static_cast<uint32>(NativeWParam));
	default:
		break;
	}

	return false;
}

bool FGameUiSystem::ProcessMouseMove(float ScreenX, float ScreenY)
{
#if WITH_RMLUI
	if (!Context)
	{
		return false;
	}

	float LocalX = 0.0f;
	float LocalY = 0.0f;
	if (!PresentationRect.ScreenToLocal(ScreenX, ScreenY, LocalX, LocalY))
	{
		Context->ProcessMouseLeave();
		return false;
	}

	const bool bNotConsumed = Context->ProcessMouseMove(static_cast<int>(LocalX), static_cast<int>(LocalY), GetRmlModifierState());
	return !bNotConsumed || WantsMouse();
#else
	(void)ScreenX;
	(void)ScreenY;
	return false;
#endif
}

bool FGameUiSystem::ProcessMouseButtonDown(int Button, float ScreenX, float ScreenY)
{
#if WITH_RMLUI
	if (!Context || !bPauseMenuVisible)
	{
		return false;
	}

	float LocalX = 0.0f;
	float LocalY = 0.0f;
	if (!PresentationRect.ScreenToLocal(ScreenX, ScreenY, LocalX, LocalY))
	{
		return false;
	}

	Context->ProcessMouseMove(static_cast<int>(LocalX), static_cast<int>(LocalY), GetRmlModifierState());
	const bool bNotConsumed = Context->ProcessMouseButtonDown(Button, GetRmlModifierState());
	return !bNotConsumed || WantsMouse();
#else
	(void)Button;
	(void)ScreenX;
	(void)ScreenY;
	return false;
#endif
}

bool FGameUiSystem::ProcessMouseButtonUp(int Button, float ScreenX, float ScreenY)
{
#if WITH_RMLUI
	if (!Context || !bPauseMenuVisible)
	{
		return false;
	}

	float LocalX = 0.0f;
	float LocalY = 0.0f;
	if (PresentationRect.ScreenToLocal(ScreenX, ScreenY, LocalX, LocalY))
	{
		Context->ProcessMouseMove(static_cast<int>(LocalX), static_cast<int>(LocalY), GetRmlModifierState());
	}
	const bool bNotConsumed = Context->ProcessMouseButtonUp(Button, GetRmlModifierState());
	return !bNotConsumed || WantsMouse();
#else
	(void)Button;
	(void)ScreenX;
	(void)ScreenY;
	return false;
#endif
}

bool FGameUiSystem::ProcessMouseWheel(float WheelDelta, float ScreenX, float ScreenY)
{
#if WITH_RMLUI
	if (!Context || !bPauseMenuVisible)
	{
		return false;
	}

	float LocalX = 0.0f;
	float LocalY = 0.0f;
	if (!PresentationRect.ScreenToLocal(ScreenX, ScreenY, LocalX, LocalY))
	{
		return false;
	}

	Context->ProcessMouseMove(static_cast<int>(LocalX), static_cast<int>(LocalY), GetRmlModifierState());
	const bool bNotConsumed = Context->ProcessMouseWheel(Rml::Vector2f(0.0f, WheelDelta), GetRmlModifierState());
	return !bNotConsumed || WantsMouse();
#else
	(void)WheelDelta;
	(void)ScreenX;
	(void)ScreenY;
	return false;
#endif
}

bool FGameUiSystem::ProcessKeyDown(int VirtualKey)
{
#if WITH_RMLUI
	if (!Context || !bPauseMenuVisible)
	{
		return false;
	}

	const Rml::Input::KeyIdentifier Key = ToRmlKey(VirtualKey);
	if (Key == Rml::Input::KI_UNKNOWN)
	{
		return false;
	}
	const bool bNotConsumed = Context->ProcessKeyDown(Key, GetRmlModifierState());
	return !bNotConsumed || WantsKeyboard();
#else
	(void)VirtualKey;
	return false;
#endif
}

bool FGameUiSystem::ProcessKeyUp(int VirtualKey)
{
#if WITH_RMLUI
	if (!Context || !bPauseMenuVisible)
	{
		return false;
	}

	const Rml::Input::KeyIdentifier Key = ToRmlKey(VirtualKey);
	if (Key == Rml::Input::KI_UNKNOWN)
	{
		return false;
	}
	const bool bNotConsumed = Context->ProcessKeyUp(Key, GetRmlModifierState());
	return !bNotConsumed || WantsKeyboard();
#else
	(void)VirtualKey;
	return false;
#endif
}

bool FGameUiSystem::ProcessTextInput(uint32 Codepoint)
{
#if WITH_RMLUI
	if (!Context || !bPauseMenuVisible || Codepoint < 32)
	{
		return false;
	}

	const bool bNotConsumed = Context->ProcessTextInput(static_cast<Rml::Character>(Codepoint));
	return !bNotConsumed || WantsKeyboard();
#else
	(void)Codepoint;
	return false;
#endif
}

bool FGameUiSystem::WantsMouse() const
{
#if WITH_RMLUI
	return Context && bPauseMenuVisible && Context->IsMouseInteracting();
#else
	return false;
#endif
}

bool FGameUiSystem::WantsKeyboard() const
{
#if WITH_RMLUI
	return Context && bPauseMenuVisible && Context->GetFocusElement() != nullptr;
#else
	return false;
#endif
}

void FGameUiSystem::UnbindPauseMenuEvents()
{
#if WITH_RMLUI
	if (!PauseMenuDocument || !EventListener)
	{
		return;
	}

	auto UnbindClick = [&](const char* ElementId)
	{
		if (Rml::Element* Element = PauseMenuDocument->GetElementById(ElementId))
		{
			Element->RemoveEventListener("click", EventListener.get(), false);
		}
	};

	UnbindClick("continue");
	UnbindClick("options");
	UnbindClick("restart");
	UnbindClick("exit");
	UnbindClick("back");
	UnbindClick("toggle-fullscreen");
	UnbindClick("toggle-fxaa");
#endif
}