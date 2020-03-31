#include "pch.h"
#include "Model.h"

namespace Engine2
{
	void Model::OnRender()
	{
		pMaterial->Bind();
		pMesh->Bind();

		for (auto& e : entities.instances)
		{
			if (e.IsActive())
			{
				e.LoadTransformT(entities.vsConstantBuffer.data.entityTransform, entities.vsConstantBuffer.data.entityTransformRotation);
				entities.vsConstantBuffer.Bind();
				pMesh->Draw();
			}
		}

		if (pMaterial->pGS) pMaterial->pGS->Unbind(); // to do: temp - do this as most things don't have a GS
	}
	void Model::OnImgui()
	{
		if (ImGui::TreeNode(name.c_str()))
		{
			ImGui::Checkbox("Active", &active);

			pMesh->OnImgui();

			pMaterial->OnImgui();

			if (ImGui::TreeNode("Entities"))
			{
				for (auto& e : entities.instances) e.OnImgui();
				ImGui::TreePop();
			}

			ImGui::TreePop();
		}
	}

}
