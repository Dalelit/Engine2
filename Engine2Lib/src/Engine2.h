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

		static void CreateEngine(HWND hwnd);
		inline static Engine& Get() { return *instance.get(); }

		Engine(HWND hwnd);
		~Engine();

		void DoFrame(float deltaTime);

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

		// to do: temp
		clock_t frameLastTime;
		static constexpr unsigned int frameTimeCount = 60;
		unsigned int frameTimeCurrent = 59;
		float frameTimes[frameTimeCount] = {};

		bool OnResize(WindowResizeEvent& event);
		void ImguiStatsWindow(bool* pOpen);
	};

}
