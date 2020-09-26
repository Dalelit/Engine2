#include "pch.h"
#include "Common.h"
#include "Entity.h"

using namespace DirectX;

namespace Engine2
{
	Entity::Entity(EngineECS::EntityId_t id, EngineECS::Coordinator& coordinator) :
		id(id), coordinator(coordinator)
	{
	}

	Entity::~Entity()
	{
	}

}
