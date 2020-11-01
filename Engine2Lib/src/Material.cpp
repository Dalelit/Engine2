#include "pch.h"
#include "Material.h"

namespace Engine2
{
	AssetStore<Material> Material::Assets;

	void Material::Bind()
	{
		vertexShaderCB->Bind();
		vertexShader->Bind();
		pixelShader->Bind();
	}

	void Material::SetTransform(Transform& transform)
	{
		vertexShaderCB->data = transform;
		vertexShaderCB->UpdateBuffer();
	}

	void Material::OnImgui(bool assetInfo)
	{
		if (ImGui::TreeNodeEx("Material", ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_DefaultOpen, "Material %s", name.c_str()))
		{
			if (assetInfo)
			{
				ImGui::Text("vertexShaderCB references %i", vertexShaderCB.use_count());
				ImGui::Text("vertexShader references %i", vertexShader.use_count());
				ImGui::Text("pixelShaderCB references %i", pixelShaderCB.use_count());
				ImGui::Text("pixelShader references %i", pixelShader.use_count());
			}

			if (vertexShaderCB) vertexShaderCB->OnImgui();
			else ImGui::Text("vertexShaderCB null");

			if (vertexShader) vertexShader->OnImgui();
			else ImGui::Text("vertexShader null");

			if (pixelShaderCB) pixelShaderCB->OnImgui();
			else ImGui::Text("pixelShaderCB null");

			if (pixelShader) pixelShader->OnImgui();
			else ImGui::Text("pixelShader null");

			ImGui::TreePop();
		}
	}
}
