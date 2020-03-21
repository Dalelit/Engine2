#pragma once

#include "Common.h"
#include "DXDevice.h"
#include "DXImgui.h"
#include "Events.h"
#include "Layer.h"

namespace Engine2 {

	class Engine
	{
	public:

		static Engine* CreateEngine(HWND hwnd);
		inline static Engine& Get() { return *instance; }

		Engine(HWND hwnd);
		~Engine();

		void DoFrame(float deltaTime);

		void OnApplicationEvent(ApplicationEvent& event);
		void OnInputEvent(InputEvent& event);
		void OnImgui();

		void ImguiActive(bool isActive = true) { imguiActive = isActive; }

	private:
		static Engine* instance;

		DXDevice device;
		DXImgui imgui;
		std::vector<Layer*> layers;
		bool minimised = false;
		bool imguiActive = false;

		// to do: temp
		float frameTime = 0.0f;
		clock_t frameLastTime;

		bool OnResize(WindowResizeEvent& event);
		void ImguiStatsWindow(bool* pOpen);
	};

}
