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
		inline static Engine& Get() { return *instance.get(); }
		inline static ID3D11Device3& GetDevice() { return instance->device.GetDevice(); }
		inline static ID3D11DeviceContext3& GetContext() { return instance->device.GetContext(); }

		Engine(HWND hwnd);
		~Engine();

		Camera mainCamera;
		InputController inputController;

		void OnUpdate(float deltaTime);
		void OnRender();

		// Engine will own the layer and delete at the end.
		void AttachLayer(Layer* layer) { layers.push_back(layer); }
		Layer& GetLayer(unsigned int indx) { return *layers[indx]; }

		void OnApplicationEvent(ApplicationEvent& event);
		void OnInputEvent(InputEvent& event);
		void OnImgui();

		void ImguiActive(bool isActive = true) { imguiActive = isActive; }

	private:
		static std::unique_ptr<Engine> instance;

		DXDevice device;
		DXImgui imgui;
		std::vector<Layer*> layers;
		bool minimised = false;
		bool imguiActive = false;

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
