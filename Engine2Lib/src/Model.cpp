#include "pch.h"
#include "Model.h"

namespace Engine2
{
	void Model::OnImgui()
	{
		if (ImGui::TreeNode(name.c_str()))
		{
			ImGui::Checkbox("Active", &active);
			ImGui::TreePop();
		}
	}

}
