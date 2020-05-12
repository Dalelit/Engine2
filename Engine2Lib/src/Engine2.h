#pragma once

#include "Common.h"
#include "DXDevice.h"
#include "Resources.h"
#include "DXImgui.h"
#include "Events.h"
#include "Layer.h"
#include "Camera.h"
#include "InputController.h"
#include "Instrumentation.h"

namespace Engine2 {

	class Engine
	{
	public:

		static void CreateEngine(HWND hwnd);
		inline static Engine& Get() { return *instance; }
		inline static Camera& GetActiveCamera() { return *instance->CurrentCamera(); }
		inline static InputController& GetInputController() { return *instance->pInputController; }

		Engine(HWND hwnd);
		~Engine();

		std::unique_ptr<InputController> pInputController;

		inline Camera* CurrentCamera() { return currentCamera; }
		inline std::vector<std::unique_ptr<Camera>>& GetCameras() { return instance->cameras; }
		unsigned int AddCamera(std::string name);
		void SetCurrentCamera(unsigned int indx);
		Camera* GetCamera(unsigned int indx);
		inline Camera* AddGetCamera(std::string name) { return GetCamera(AddCamera(name)); }

		void DoFrame(float deltaTime); // does update then render
		void Update(float deltaTime);
		void Render();

		// Engine will own the layer and delete at the end.
		void AttachLayer(Layer* layer, bool active = true) { layers.push_back(layer); if (!active) { layer->SetActive(false); layer->imguiOpen = false; } }
		Layer& GetLayer(unsigned int indx) { return *layers[indx]; }

		void OnApplicationEvent(ApplicationEvent& event);
		void OnInputEvent(InputEvent& event);
		void OnImgui();

		void ImguiActive(bool isActive = true) { imguiActive = isActive; }

	private:
		static std::unique_ptr<Engine> instance;

		DXDevice& device;
		DXImgui imgui;
		std::vector<Layer*> layers;
		bool minimised = false;
		bool imguiActive = false;

		std::vector<std::unique_ptr<Camera>> cameras;
		Camera* currentCamera;
		unsigned int currentCameraIndx;
		void ImuguiCameraWindow(bool* pOpen);

		// application event handlers
		bool OnResize(WindowResizeEvent& event);

		void ImguiStatsWindow(bool* pOpen);

		Instrumentation::Timer frameRate;
		Instrumentation::Timer frameTime;
		Instrumentation::MemoryTracker updateMemory;
		Instrumentation::MemoryTracker renderMemory;
		Instrumentation::MemoryTracker renderLayersMemory;
		Instrumentation::MemoryTracker renderImguiMemory;
	};

}
