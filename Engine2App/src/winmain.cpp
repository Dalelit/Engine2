#include "pch.h"
#include "Engine2.h"
#include "BallWorld.h"
#include "ModelTest.h"
#include "ParticleTest.h"
#include "BlockWorld.h"
#include "ECSTest.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>

#define E2_OUTPUT_DEBUG(msg) { OutputDebugStringA(msg); OutputDebugStringA("\n"); }

class AppWindow
{
public:
	AppWindow(HINSTANCE hInstance, WNDPROC wndProc)
	{
		Regsiter(hInstance, wndProc);
		Create(hInstance);
		SetupRawMouseIntput();
	}

	~AppWindow()
	{
		Unregsiter();
	}

	void Maximise() { ShowWindow(handle, SW_SHOWMAXIMIZED); }
	void Hide()     { ShowWindow(handle, SW_HIDE); }
	void Restore()  { ShowWindow(handle, SW_RESTORE); }

	HWND GetHandle() { return handle; }

	std::string GetDirectory() {
		char buffer[256];
		GetCurrentDirectoryA(ARRAYSIZE(buffer), buffer);
		return std::string(buffer);
	}

private:
	HWND handle;
	WNDCLASS windowClass = {};

	void Regsiter(HINSTANCE hInstance, WNDPROC wndProc)
	{
		windowClass = { 0 };
		windowClass.style = CS_OWNDC;
		windowClass.lpfnWndProc = wndProc;
		windowClass.hInstance = hInstance;
		windowClass.hbrBackground = (HBRUSH)(COLOR_BACKGROUND);
		windowClass.lpszClassName = L"Engine2";

		HRESULT hr = RegisterClass(&windowClass);
	}

	void SetupRawMouseIntput()
	{
		RAWINPUTDEVICE rid;
		rid.usUsagePage = 0x01; // mouse page
		rid.usUsage = 0x02; // mouse usage
		rid.dwFlags = 0;
		rid.hwndTarget = nullptr;
		if (!RegisterRawInputDevices(&rid, 1u, sizeof(rid))) { E2_OUTPUT_DEBUG("Error with RegisterRawInputDevices"); }
	}

	void Unregsiter()
	{
		UnregisterClass(windowClass.lpszClassName, windowClass.hInstance);
	}

	void Create(HINSTANCE hInstance)
	{
		handle = CreateWindow(windowClass.lpszClassName,
			L"Engine2 App",
			WS_OVERLAPPEDWINDOW | WS_VISIBLE,
			0, 0,
			2*800, 2*600,
			0, // parent window
			0, hInstance, NULL);
	}
};

// Functions to call to raise events to the engine. Once the engine is instantiated, the lambda will get replaced with the engine function.
std::function<void(Engine2::ApplicationEvent&)> OnApplicationEventFunc = [](Engine2::ApplicationEvent& event) { E2_OUTPUT_DEBUG(event.GetName()); };
std::function<void(Engine2::InputEvent&)> OnInputEventFunc = [](Engine2::InputEvent& event) { E2_OUTPUT_DEBUG(event.GetName()); };

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (Engine2::DXImgui::WndProcHandler(hWnd, msg, wParam, lParam))
		return 0;

	switch (msg)
	{
		// window controls /////////////////////////
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			break;
		}
		case WM_CLOSE:
		{
			PostQuitMessage(0);
			break;
		}
		case WM_SIZE:
		{
			Engine2::WindowResizeEvent event(wParam == SIZE_MINIMIZED, wParam == SIZE_MAXIMIZED);
			OnApplicationEventFunc(event);
			break;
		}
		case WM_MOVE:
		{
			Engine2::WindowMoveEvent event((int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam));
			OnApplicationEventFunc(event);
			break;
		}
		case WM_ACTIVATE:
		{
			Engine2::WindowFocusEvent event(wParam != WA_INACTIVE, wParam == WA_CLICKACTIVE);
			OnApplicationEventFunc(event);
			break;
		}

		// key controls /////////////////////////
		case WM_KEYDOWN:
		{
			if (wParam == VK_ESCAPE) PostQuitMessage(0); // to do: put exit controls into the engine

			Engine2::KeyPressedEvent event((UINT32)wParam, (UINT32)(lParam & 0x00ff));
			OnInputEventFunc(event);
			break;
		}

		// mouse controls /////////////////////////
		case WM_INPUT:
		{
			BYTE rawBuffer[64];
			UINT size;

			if (GetRawInputData((HRAWINPUT)lParam, RID_INPUT, nullptr, &size, sizeof(RAWINPUTHEADER)) == -1) break;
			if (size == 0) break;

			E2_ASSERT(size <= ARRAYSIZE(rawBuffer), "rawBuffer smaller than rawInputSize");

			if (GetRawInputData((HRAWINPUT)lParam, RID_INPUT, rawBuffer, &size, sizeof(RAWINPUTHEADER)) != size) E2_OUTPUT_DEBUG("Raw buffer read did not match");

			RAWINPUT* raw = (RAWINPUT*)rawBuffer;

			if (raw->header.dwType == RIM_TYPEMOUSE)
			{
				if (raw->data.mouse.lLastX != 0 || raw->data.mouse.lLastY != 0) // remove 0,0 events
				{
					Engine2::MouseMoveEvent event((int)raw->data.mouse.lLastX, (int)raw->data.mouse.lLastY);
					OnInputEventFunc(event);
				}
			}

			break;
		}
		case WM_LBUTTONDOWN:
		{
			Engine2::MouseButtonPressedEvent event(true, false, false, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			OnInputEventFunc(event);
			break;
		}
		case WM_LBUTTONUP:
		{
			Engine2::MouseButtonReleasedEvent event(true, false, false, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			OnInputEventFunc(event);
			break;
		}
		case WM_RBUTTONDOWN:
		{
			Engine2::MouseButtonPressedEvent event(false, true, false, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			OnInputEventFunc(event);
			break;
		}
		case WM_RBUTTONUP:
		{
			Engine2::MouseButtonReleasedEvent event(false, true, false, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			OnInputEventFunc(event);
			break;
		}
		case WM_MBUTTONDOWN:
		{
			Engine2::MouseButtonPressedEvent event(false, false, true, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			OnInputEventFunc(event);
			break;
		}
		case WM_MBUTTONUP:
		{
			Engine2::MouseButtonReleasedEvent event(false, false, true, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			OnInputEventFunc(event);
			break;
		}
		case WM_MOUSEWHEEL:
		{
			Engine2::MouseScrollEvent event(GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			OnInputEventFunc(event);
			break;
		}

		default:
			return DefWindowProc(hWnd, msg, wParam, lParam);
	}

	return 0;
}

int WINAPI WinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR lpCmdLine,
	_In_ int nShowCmd
)
{
	bool running = true;

	AppWindow app(hInstance, WndProc);
	//app.Maximise();
	E2_OUTPUT_DEBUG(app.GetDirectory().c_str());

	Engine2::Engine::CreateEngine(app.GetHandle());

	OnApplicationEventFunc = [](Engine2::ApplicationEvent& event) { Engine2::Engine::Get().OnApplicationEvent(event); };
	OnInputEventFunc = [](Engine2::InputEvent& event) { Engine2::Engine::Get().OnInputEvent(event); };

	// reminder: camera position is set by last layer
	//Engine2::Engine::Get().AttachLayer(new ModelTest(), false);
	//Engine2::Engine::Get().AttachLayer(new BallWorld(), false);
	//Engine2::Engine::Get().AttachLayer(new ParticleTest(), false);
	//Engine2::Engine::Get().AttachLayer(new BlockWorld::BlockWorld(), false);
	Engine2::Engine::Get().AttachLayer(new ECSTest());

	clock_t lastTime = clock();

	while (running)
	{
		MSG msg;

		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			if (msg.message == WM_QUIT) running = false;
		}

		clock_t currentTime = clock();
		float delatTimeSeconds = (float)(currentTime - lastTime) / (float)CLOCKS_PER_SEC;

		Engine2::Engine::Get().DoFrame(delatTimeSeconds);

		lastTime = currentTime;
	}
}
