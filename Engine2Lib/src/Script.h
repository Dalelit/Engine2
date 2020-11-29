#pragma once
#include "Events.h"
#include "Entity.h"

namespace Engine2
{
	class Script
	{
	public:
		// Override the below methods as required

		virtual void OnInitialise() {} // called after it is created in the ScriptComponent
		virtual void OnUpdate(float dt) {}
		virtual void OnImgui() {} // Override with the body of the script UI.
		//virtual void OnDrawGizmo() {}
		//virtual void OnInputEvent(InputEvent& event) {}
		//virtual void OnApplicationEvent(ApplicationEvent& event) {}

		virtual bool IsValid() { return true; } // Override if you need to check for SetActive. E.g. check entity has required components

		// Called by the engine, can be called by scripts

		inline bool IsActive() { return active; } // called after OnInitialise or via the UI
		inline void SetActive() { active = IsValid(); }
		inline void SetInactive() { active = false; }

		inline const std::string& Name() const { return name; }

		// the below is called by the ScriptComponent when instantiated, prior to calling OnInitialise()
		inline void SetEntity(Entity& e) { entity = e; }
		inline void SetName(const std::string& newName) { name = newName; }

	protected:
		bool active = true;
		Entity entity;
		std::string name;
	};
}
