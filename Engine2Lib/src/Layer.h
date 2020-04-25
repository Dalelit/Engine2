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

		inline std::string GetName() const { return name; }

		inline bool IsActive() const { return active; }
		inline void SetActive(bool makeActive = true) { active = makeActive; }

		virtual void OnUpdate(float dt) {}                          // override this to update scene prior to rendering
		virtual void OnRender() {}                                  // override this with rendering logic
		virtual void OnInputEvent(InputEvent& event) {}             // handle input events
		virtual void OnApplicationEvent(ApplicationEvent& event) {} // handle application events
		virtual void OnImgui() {};                                  // override this as the imgui window content for the layer
		virtual void OnGizmos() {};                                 // override this to draw any gizmos

		// Below are called from the engine

		inline void Render() { OnRender(); }

		inline void RenderGizmos()
		{
			if (gizmos.IsActive())
			{
				gizmos.NewFrame();
				OnGizmos();
				gizmos.Render();
			}
		}

		bool imguiOpen = true;
		inline void ImguiWindow()
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
