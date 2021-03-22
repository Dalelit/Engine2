#pragma once

#include <string>
#include "ECS.h"
#include "Entity.h"

namespace Engine2
{
	class Component
	{
	public:
		void SetEntity(const Entity& e) { entity = e; }

		template <typename T, typename... ARGS>
		T* AddComponent(ARGS... args) { return entity.AddComponent<T>(args...); }

		template <typename T>
		T* GetComponent() { return entity.GetComponent<T>(); }

		template <typename T>
		bool HasComponent() { return entity.HasComponent<T>(); }

	protected:
		Entity entity;
	};

	class Components
	{
	public:
		static void OnImgui(EngineECS::EntityId_t id, EngineECS::Coordinator& coord);
	};
}