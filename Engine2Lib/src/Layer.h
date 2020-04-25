#pragma once
#include "pch.h"
#include "Common.h"
#include "Events.h"
#include "Gizmos.h"

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

		void Render()
		{
			gizmos.NewFrame();
			OnRender();
			if (gizmos.IsActive()) gizmos.Render();
		}

		// called from the engine
		bool imguiOpen = true;
		void ImguiWindow()
		{
			if (imguiOpen)
			{
				ImGui::Begin(name.c_str(), &imguiOpen);
				ImGui::Checkbox("Active", &active);
				gizmos.OnImgui();
				OnImgui();
				ImGui::End();
			}
		}

	protected:
		std::string name;
		bool active = true;

		Gizmos gizmos;
	};

}
