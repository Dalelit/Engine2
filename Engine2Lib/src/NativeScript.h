#pragma once
#include "Events.h"
#include "Entity.h"

namespace Engine2
{
	class NativeScript
	{
	public:

		void SetEntity(Entity& e) { entity = e; }

		virtual void OnInitialise() {}
		virtual void OnUpdate(float dt) {}
		virtual void OnInputEvent(InputEvent& event) {}
		virtual void OnApplicationEvent(ApplicationEvent& event) {}
		virtual void OnDrawGizmo() {}
		virtual void OnImgui() {}

		inline void SetActive() { active = true; }
		inline void SetInactive() { active = false; }

	protected:
		Entity entity;
		bool active = true;
	};
}