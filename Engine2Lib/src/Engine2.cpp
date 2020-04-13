#include "pch.h"
#include "Common.h"
#include "Engine2.h"
#include "Instrumentation.h"

namespace Engine2
{
	std::unique_ptr<Engine> Engine::instance = nullptr;

	void Engine::CreateEngine(HWND hwnd)
	{
		E2_ASSERT(instance == nullptr, "Engine instance already exists");

		instance = std::make_unique<Engine>(hwnd);
	}

	Engine::Engine(HWND hwnd) :
		device(hwnd), inputController(&mainCamera)
	{
		imgui.Initialise(hwnd, device);
		imguiActive = true;

		mainCamera.SetAspectRatio(device.GetAspectRatio());
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
		Instrumentation::FrameReset();
		OnUpdate(deltaTime);
		OnRender();
	}

	void Engine::OnUpdate(float deltaTime)
	{
		updateMemory.Set();
		frameTime.Set(); // Tick just before present frame

		inputController.OnUpdate(deltaTime);

		// update layers
		for (auto layer : layers)
		{
			if (layer->IsActive()) layer->OnUpdate(deltaTime);
		}

		updateMemory.Tick();
	}

	void Engine::OnRender()
	{
		renderMemory.Set();

		// dx begin
		device.BeginFrame();

		renderLayersMemory.Set();

		// render layers
		if (!minimised)
		{
			for (auto& layer : layers)
			{
				if (layer->IsActive()) layer->OnRender();
			}
		}
		renderLayersMemory.Tick();

		// render imgui
		renderImguiMemory.Set();

		if (imguiActive)
		{
			imgui.BeginFrame();

			OnImgui(); // engine's own Imgui

			for (auto& layer : layers) layer->ImguiWindow();

			imgui.EndFrame();
		}
		renderImguiMemory.Tick();

		frameTime.Tick();

		// dx present
		device.PresentFrame();

		frameRate.Tick();
		renderMemory.Tick();
	}

	void Engine::OnApplicationEvent(ApplicationEvent& event)
	{
		inputController.OnApplicationEvent(event);

		EventDispatcher dispacher(event);

		dispacher.Dispatch<WindowResizeEvent>(E2_BIND_EVENT_FUNC(Engine::OnResize));

		for (auto layer : layers)
		{
			layer->OnApplicationEvent(event);
		}
	}

	void Engine::OnInputEvent(InputEvent& event)
	{
		// check if ImGui handled the event
		if (event.GetGroup() == EventGroup::Mouse && ImGui::GetIO().WantCaptureMouse) return;
		if (event.GetGroup() == EventGroup::Keyboard && ImGui::GetIO().WantCaptureKeyboard) return;

		inputController.OnInputEvent(event);

		//EventDispatcher dispacher(event);

		for (auto layer : layers)
		{
			if (layer->IsActive()) layer->OnInputEvent(event);
		}
	}

	bool Engine::OnResize(WindowResizeEvent& event)
	{
		if (event.IsMinimised())
		{
			minimised = true;
		}
		else
		{
			minimised = false;
			device.ScreenSizeChanged();
			mainCamera.SetAspectRatio(device.GetAspectRatio());
		}

		return true;
	}

	void Engine::OnImgui()
	{
		static bool demoOpen = false;
		if (demoOpen) ImGui::ShowDemoWindow(&demoOpen);

		static bool statsOpen = true;
		if (statsOpen) ImguiStatsWindow(&statsOpen);

		static bool memoryOpen = true;
		if (memoryOpen) Instrumentation::Memory::ImguiWindow(&memoryOpen);

		static bool drawStatsOpen = true;
		if (drawStatsOpen) Instrumentation::Drawing::ImguiWindow(&drawStatsOpen);

		static bool inputControllerOpen = true;
		if (inputControllerOpen) inputController.ImguiWindow(&inputControllerOpen);

		static bool cameraOpen = true;
		if (cameraOpen) mainCamera.ImuguiWindow(&cameraOpen);

		if (ImGui::Begin("Engine2", nullptr, ImGuiWindowFlags_MenuBar))
		{
			if (ImGui::BeginMenuBar())
			{
				if (ImGui::BeginMenu("Menu"))
				{
					ImGui::MenuItem("Demo window", nullptr, &demoOpen);
					ImGui::MenuItem("Stats overlay", nullptr, &statsOpen);
					ImGui::MenuItem("Memory overlay", nullptr, &memoryOpen);
					ImGui::MenuItem("Draw stats overlay", nullptr, &drawStatsOpen);
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
			ImGui::Text("Frame rate %.1f", (1000.0f / frameRate.Average()));
			frameRate.OnImgui();
			ImGui::Text("Frame time");
			frameTime.OnImgui();
			ImGui::Text("Update mallocs %.1f", updateMemory.allocations.Average());
			ImGui::Text("Render mallocs %.1f", renderMemory.allocations.Average());
			ImGui::Text(" layer mallocs %.1f", renderLayersMemory.allocations.Average());
			ImGui::Text(" Imgui mallocs %.1f", renderImguiMemory.allocations.Average());
			ImGui::End();
		}

		ImGui::GetStyle().Colors[ImGuiCol_FrameBg] = bg; // recover the background color
	}

}
