#include "pch.h"
#include "Model.h"

namespace Engine2
{
	void Model::Draw()
	{
		pMaterial->Bind();
		pMesh->Bind();
		pMesh->Draw();

		pMaterial->UnbindAfterDraw();
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

	void ModelEntities::Draw()
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

		pMaterial->UnbindAfterDraw();
	}

	void ModelEntities::OnImgui()
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
