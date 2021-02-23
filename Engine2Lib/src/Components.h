#pragma once

#include <string>
#include "ECS.h"
#include "Serialiser.h"

namespace Engine2
{
	struct EntityInfo
	{
		std::string tag;

		void OnImgui();

		void Serialise(Serialisation::INode& node);
	};

	class Components
	{
	public:
		static void OnImgui(EngineECS::EntityId_t id, EngineECS::Coordinator& coord);
	};
}