#include "pch.h"
#include "Common.h"
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
		imgui.Initialise(hwnd, device);
		imguiActive = true;

		frameLastTime = clock();
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
			imgui.BeginFrame();

			OnImgui(); // engine's own Imgui

			for (auto layer : layers)
			{
				layer->OnImgui();
			}

			imgui.EndFrame();
		}

		// dx present
		device.PresentFrame();

		// to do: temp
		clock_t currentTime = clock();
		frameTime = (float)(currentTime - frameLastTime);
		frameLastTime = currentTime;
	}

	void Engine::OnApplicationEvent(ApplicationEvent& event)
	{
		EventDispatcher dispacher(event);

		dispacher.Dispatch<WindowResizeEvent>(E2_BIND_EVENT_FUNC(Engine::OnResize));
	}

	void Engine::OnInputEvent(InputEvent& event)
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

	void Engine::OnImgui()
	{
		static bool demoOpen = true;
		if (demoOpen) ImGui::ShowDemoWindow(&demoOpen);

		static bool statsOpen = true;
		if (statsOpen) ImguiStatsWindow(&statsOpen);

		if (ImGui::Begin("Engine2", nullptr, ImGuiWindowFlags_MenuBar))
		{
			if (ImGui::BeginMenuBar())
			{
				if (ImGui::BeginMenu("Menu"))
				{
					ImGui::MenuItem("Demo window", NULL, &demoOpen);
					ImGui::MenuItem("Stats overlay", NULL, &statsOpen);
					ImGui::EndMenu();
				}
				ImGui::EndMenuBar();
			}
		}

		ImGui::End();
	}

	void Engine::ImguiStatsWindow(bool* pOpen)
	{
		ImGui::SetNextWindowBgAlpha(0.35f); // Transparent background
		if (ImGui::Begin("Stats", pOpen, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text("Frame time: %.0f ms", frameTime);
			ImGui::End();
		}
	}

}
