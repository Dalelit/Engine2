#include "pch.h"
#include "EntityInfo.h"

namespace Engine2
{
	void EntityInfo::OnImgui()
	{
		char buffer[256] = {};
		strcpy_s(buffer, sizeof(buffer), tag.c_str());
		if (ImGui::InputText("Tag", buffer, sizeof(buffer)))
		{
			tag = buffer;
		}
	}

	void EntityInfo::Serialise(Serialisation::INode& node)
	{
		node.Attribute("tag", tag);
	}
}