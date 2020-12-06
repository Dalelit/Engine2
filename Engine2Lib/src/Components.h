#pragma once

#include <string>
#include "ECS.h"

namespace Engine2
{
	struct EntityInfo
	{
		std::string tag;

		void OnImgui();
	};

	class Components
	{
	public:
		static void OnImgui(EngineECS::EntityId_t id, EngineECS::Coordinator& coord);
	};
}