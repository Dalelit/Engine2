#include "pch.h"
#include "Material.h"

namespace Engine2
{
	void Material::Bind()
	{
		pVS->Bind();
		pPS->Bind();

		for (auto& r : resources) r->Bind();
	}
	void Material::OnImgui()
	{
		ImGui::Text(name.c_str());

		if (ImGui::TreeNode("Vertex shader"))
		{
			pVS->OnImgui();
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Pixel shader"))
		{
			pPS->OnImgui();
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Resources"))
		{
			for (auto& r : resources) if (r->ImguiFunc) r->ImguiFunc();
			ImGui::TreePop();
		}
	}
}

