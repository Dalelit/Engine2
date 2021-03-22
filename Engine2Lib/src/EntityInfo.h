#pragma once
#include "Serialiser.h"

namespace Engine2
{
	struct EntityInfo
	{
		std::string tag;

		void OnImgui();

		void Serialise(Serialisation::INode& node);
	};

}