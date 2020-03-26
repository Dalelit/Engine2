#include "pch.h"
#include "Common.h"
#include "Engine2.h"

namespace Engine2
{
	std::unique_ptr<Engine> Engine::instance = nullptr;

	void Engine::CreateEngine(HWND hwnd)
	{
		E2_ASSERT(instance == nullptr, "Engine instance already exists");

		instance = std::make_unique<Engine>(hwnd);
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

	void Engine::OnUpdate(float deltaTime)
	{
		// update layers
		for (auto layer : layers)
		{
			if (layer->IsActive()) layer->OnUpdate(deltaTime);
		}
	}

	void Engine::OnRender()
	{
		// dx begin
		device.BeginFrame();

		// render layers
		if (!minimised)
		{
			for (auto layer : layers)
			{
				if (layer->IsActive()) layer->OnRender();
			}
		}

		// render imgui
		if (imguiActive)
		{
			imgui.BeginFrame();

			OnImgui(); // engine's own Imgui

			for (auto layer : layers) layer->ImguiWindow();

			imgui.EndFrame();
		}

		// dx present
		device.PresentFrame();

		// to do: temp
		clock_t currentTime = clock();
		frameTimeCurrent++;  if (frameTimeCurrent >= frameTimeCount) frameTimeCurrent = 0;
		frameTimes[frameTimeCurrent] = (float)(currentTime - frameLastTime);
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
		static bool demoOpen = false;
		if (demoOpen) ImGui::ShowDemoWindow(&demoOpen);

		static bool statsOpen = true;
		if (statsOpen) ImguiStatsWindow(&statsOpen);

		if (ImGui::Begin("Engine2", nullptr, ImGuiWindowFlags_MenuBar))
		{
			if (ImGui::BeginMenuBar())
			{
				if (ImGui::BeginMenu("Menu"))
				{
					ImGui::MenuItem("Demo window", nullptr, &demoOpen);
					ImGui::MenuItem("Stats overlay", nullptr, &statsOpen);
					ImGui::EndMenu();
				}
				ImGui::EndMenuBar();
			}

			if (ImGui::CollapsingHeader("Layer windows"))
			{
				for (auto layer : layers) ImGui::Checkbox(layer->GetName().c_str(), &layer->imguiOpen);
			}
		}

		ImGui::End();
	}

	void Engine::ImguiStatsWindow(bool* pOpen)
	{
		ImGui::SetNextWindowBgAlpha(0.35f); // Transparent background
		ImVec4 bg = ImGui::GetStyleColorVec4(ImGuiCol_FrameBg); // store the current background color
		ImGui::GetStyle().Colors[ImGuiCol_FrameBg] = { 0,0,0,0 }; // set background to transparent

		if (ImGui::Begin("Stats", pOpen, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize))
		{
			float avg = 0.0f;
			for (int i = 0; i < frameTimeCount; i++) avg += frameTimes[i];
			avg /= frameTimeCount;
			char buffer[13];
			sprintf_s(buffer, ARRAYSIZE(buffer), "FR ms %.1f", avg);
			ImGui::PlotLines("", frameTimes, frameTimeCount, frameTimeCurrent, buffer, 0.0f, 30.0f, {0,40});

			ImGui::End();
		}

		ImGui::GetStyle().Colors[ImGuiCol_FrameBg] = bg; // recover the background color
	}

}
