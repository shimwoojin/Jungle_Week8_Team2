#pragma once

#include "Core/CoreTypes.h"
#include "Viewport/ViewportPresentationTypes.h"

#include <cstdint>
#include <functional>
#include <memory>

// Keep <windows.h> out of this public header. Several Win32 helper macros
// collide with RmlUi headers when this header is included transitively.

class FRenderer;
class FWindowsWindow;
class UGameViewportClient;
class FRmlD3D11RenderInterface;
class FRmlWin32SystemInterface;
class FGameUiEventListener;

namespace Rml
{
	class Context;
	class ElementDocument;
}

struct FGameUiCallbacks
{
	std::function<void()> OnContinue;
	std::function<void()> OnRestart;
	std::function<void()> OnExit;
	std::function<void()> OnToggleFullscreen;
	std::function<bool()> IsFullscreen;
	std::function<void(bool)> OnFxaaChanged;
	std::function<bool()> IsFxaaEnabled;
};

// 공통 게임 UI 레이어.
// Standalone Client와 Editor PIE가 같은 UGameViewportClient/PresentationRect를 통해 동일한 HUD와 메뉴를 사용한다.
class FGameUiSystem
{
public:
	FGameUiSystem();
	~FGameUiSystem();

	bool Initialize(FWindowsWindow* Window, FRenderer& Renderer, UGameViewportClient* InViewportClient);
	void Shutdown();

	void SetCallbacks(FGameUiCallbacks InCallbacks);
	void SetPresentationRect(const FViewportPresentationRect& InRect);
	const FViewportPresentationRect& GetPresentationRect() const { return PresentationRect; }

	void Update(float DeltaTime);
	void Render();

	void SetPauseMenuVisible(bool bVisible);
	bool IsPauseMenuVisible() const { return bPauseMenuVisible; }
	bool IsAvailable() const { return bAvailable; }
	bool IsInitialized() const { return bInitialized; }

	bool ProcessWin32Message(void* hWnd, uint32 Msg, std::uintptr_t wParam, std::intptr_t lParam);
	bool ProcessMouseMove(float ScreenX, float ScreenY);
	bool ProcessMouseButtonDown(int Button, float ScreenX, float ScreenY);
	bool ProcessMouseButtonUp(int Button, float ScreenX, float ScreenY);
	bool ProcessMouseWheel(float WheelDelta, float ScreenX, float ScreenY);
	bool ProcessKeyDown(int VirtualKey);
	bool ProcessKeyUp(int VirtualKey);
	bool ProcessTextInput(uint32 Codepoint);

	bool WantsMouse() const;
	bool WantsKeyboard() const;
	void UnbindPauseMenuEvents();

private:
	friend class FGameUiEventListener;

	bool LoadDocuments();
	void BindPauseMenuEvents();
	void HandleClick(const FString& ElementId);
	void SetOptionsVisible(bool bVisible);
	void RefreshOptionLabels();
	void SetElementDisplay(const char* ElementId, bool bVisible);
	void SetElementText(const char* ElementId, const char* Text);
	void SyncContextDimensions();

private:
	FWindowsWindow* OwnerWindow = nullptr;
	UGameViewportClient* ViewportClient = nullptr;
	FViewportPresentationRect PresentationRect;
	FGameUiCallbacks Callbacks;

	std::unique_ptr<FRmlD3D11RenderInterface> RenderInterface;
	std::unique_ptr<FRmlWin32SystemInterface> SystemInterface;
	std::unique_ptr<FGameUiEventListener> EventListener;

	Rml::Context* Context = nullptr;
	Rml::ElementDocument* HudDocument = nullptr;
	Rml::ElementDocument* PauseMenuDocument = nullptr;

	bool bInitialized = false;
	bool bAvailable = false;
	bool bPauseMenuVisible = false;
	bool bShowingOptions = false;
};
