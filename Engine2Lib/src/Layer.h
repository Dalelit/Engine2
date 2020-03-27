#pragma once
#include "pch.h"
#include "Common.h"
#include "Events.h"

namespace Engine2
{

	class Layer
	{
	public:
		Layer(std::string name) : name(name) {}
		virtual ~Layer() = default;

		std::string GetName() const { return name; }

		inline bool IsActive() const { return active; }
		inline void SetActive(bool makeActive = true) { active = makeActive; }

		virtual void OnUpdate(float dt) {}
		virtual void OnRender() {}
		virtual void OnInputEvent(InputEvent& event) {}
		virtual void OnApplicationEvent(ApplicationEvent& event) {}
		virtual void OnImgui() {}; // override this as the imgui window content for the layer

		// called from the engine
		void ImguiWindow() { if (imguiOpen) { ImGui::Begin(name.c_str(), &imguiOpen); OnImgui(); ImGui::End(); } }
		bool imguiOpen = true;

	protected:
		std::string name;
		bool active = true;
	};

}
