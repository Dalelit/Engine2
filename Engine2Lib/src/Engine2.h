#pragma once

#include "DXDevice.h"
#include "Events.h"
#include "Layer.h"

#define E2_ASSERT(x, msg) assert(x)

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

		void ImguiActive(bool makeImguiActive = true) { imguiActive = makeImguiActive; }

	private:
		static Engine* instance;

		DXDevice device;
		std::vector<Layer*> layers;
		bool minimised = false;
		bool imguiActive = true;

		bool OnResize(WindowResizeEvent& event);
	};

}
