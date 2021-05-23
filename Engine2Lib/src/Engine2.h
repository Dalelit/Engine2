#pragma once

#include "Common.h"
#include "DXDevice.h"
#include "Resources.h"
#include "DXImgui.h"
#include "Events.h"
#include "Layer.h"
#include "Instrumentation.h"
#include "InputController.h"
#include "UID.h"

namespace Engine2 {

	class Engine
	{
	public:

		static void CreateEngine(HWND hwnd);
		inline static Engine& Get() { return *instance; }
		inline const InputController::InputState& InputState() { return inputController->State; }

		Engine(HWND hwnd);
		~Engine();

		void DoFrame(float deltaTime); // does update then render
		void Update(float deltaTime);
		void Render();

		// Engine will own the layer and delete at the end.
		void AttachLayer(Layer* layer, bool active = true) { layers.push_back(layer); if (!active) { layer->SetActive(false); layer->imguiOpen = false; } }

		template <typename T>
		void AttachLayer() { AttachLayer(new T()); }

		Layer& GetLayer(unsigned int indx) { return *layers[indx]; }

		template <typename T>
		void RegisterLayer(const std::string& layerTitle, bool instantiate = false) {
			if (instantiate) AttachLayer<T>();
			else layerRegister[layerTitle] = [this]() { this->AttachLayer<T>(); };
		}

		void OnApplicationEvent(ApplicationEvent& event);
		void OnInputEvent(InputEvent& event);
		void OnImgui();

		void ImguiActive(bool isActive = true) { imguiActive = isActive; }

		UID GenerateId() { return idGenerator.Next(); } // note: really basic implementation - not thread safe.

	private:
		static std::unique_ptr<Engine> instance;
		std::unique_ptr<InputController> inputController;

		DXDevice& device;
		DXImgui imgui;
		std::vector<Layer*> layers;
		bool minimised = false;
		bool imguiActive = false;

		std::map<std::string, std::function<void()>> layerRegister;

		// application event handlers
		bool OnResize(WindowResizeEvent& event);

		void ImguiStatsWindow(bool* pOpen);

		Instrumentation::Timer frameRate;
		Instrumentation::Timer frameTime;
		Instrumentation::MemoryTracker updateMemory;
		Instrumentation::MemoryTracker renderMemory;
		Instrumentation::MemoryTracker renderLayersMemory;
		Instrumentation::MemoryTracker renderImguiMemory;

		UIDGenerator idGenerator;
	};

}
