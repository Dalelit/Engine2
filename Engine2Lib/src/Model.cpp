#include "pch.h"
#include "Model.h"

namespace Engine2
{
	void Model::Bind()
	{
		pMaterial->Bind();
		pMesh->Bind();
	}
	void Model::OnImgui()
	{
		if (ImGui::TreeNode(name.c_str()))
		{
			ImGui::Checkbox("Active", &active);

			pMesh->OnImgui();

			pMaterial->OnImgui();

			ImGui::TreePop();
		}
	}

}
