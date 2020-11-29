#pragma once
//#include "pch.h"
#include "ECS.h"

namespace Engine2
{
	class Entity
	{
	public:
		Entity() = default;
		Entity(EngineECS::EntityId_t id, EngineECS::Coordinator& coordinator) : id(id), coordinator(&coordinator) {}

		Entity& operator=(const Entity& rhs) { id = rhs.id; coordinator = rhs.coordinator; return *this; }

		EngineECS::EntityId_t Id() { return id; }

		template <typename T>
		T* AddComponent() { return coordinator->AddComponent<T>(id); }

		template <typename T>
		T* GetComponent() { return coordinator->GetComponent<T>(id); }

		template <typename T>
		bool HasComponent() { return coordinator->HasComponent<T>(id); }

		bool IsValid() { return coordinator != nullptr; }

	protected:
		EngineECS::EntityId_t id = 0;
		EngineECS::Coordinator* coordinator = nullptr;
	};
}