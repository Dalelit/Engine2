#include "pch.h"
#include "Engine2.h"

namespace Engine2
{
	Engine* Engine::instance = nullptr;

	Engine* Engine::CreateEngine(HWND hwnd)
	{
		E2_ASSERT(instance == nullptr, "Engine instance already exists");

		instance = new Engine(hwnd);

		return instance;
	}
	Engine::Engine(HWND hwnd) :
		device(hwnd)
	{
	}

	Engine::~Engine()
	{
		for (auto layer : layers)
		{
			delete layer;
		}
	}

	void Engine::DoFrame(float deltaTime)
	{
		// dx begin
		device.BeginFrame();

		// update layers
		for (auto layer : layers)
		{
			layer->OnUpdate(deltaTime);
		}

		// render layers
		if (!minimised)
		{
			for (auto layer : layers)
			{
				layer->OnRender();
			}
		}

		// render imgui
		if (imguiActive)
		{
			//to do being

			OnImgui(); // engine's own Imgui

			for (auto layer : layers)
			{
				layer->OnImgui();
			}

			//to do end
		}

		// dx present
		device.PresentFrame();
	}

	void Engine::OnApplicationEvent(ApplicationEvent& event)
	{
		EventDispatcher dispacher(event);

		dispacher.Dispatch<WindowResizeEvent>(E2_BIND_EVENT_FUNC(Engine::OnResize));
	}

	void Engine::OnInputEvent(InputEvent& event)
	{
	}

	void Engine::OnImgui()
	{
	}

	bool Engine::OnResize(WindowResizeEvent& event)
	{
		if (event.GetWidth() > 0 && event.GetHeight() > 0)
		{
			device.ScreenSizeChanged();
			minimised = false;
		}
		else
		{
			minimised = true;
		}

		return true;
	}

}
