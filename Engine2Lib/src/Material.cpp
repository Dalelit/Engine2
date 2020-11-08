#include "pch.h"
#include "Material.h"

namespace Engine2
{
	AssetStore<Material>         Material::Materials;
	AssetStore<VertexShaderFile> Material::VertexShaders;
	AssetStore<PixelShaderFile>  Material::PixelShaders;

	void Material::Bind()
	{
		vertexShaderCB->Bind();
		vertexShader->Bind();
		pixelShaderCB->Bind();
		pixelShader->Bind();
	}

	void Material::SetTransform(TransformMatrix& transform)
	{
		vertexShaderCB->data = transform;
		vertexShaderCB->UpdateBuffer();
	}

	std::shared_ptr<Material> Material::Clone(const std::string& cloneName)
	{
		auto ptr = std::make_shared<Material>(cloneName);

		ptr->vertexShaderCB = std::make_shared<VSConstantBuffer<TransformMatrix>>(*vertexShaderCB); // vertexShaderCB->Clone();
		ptr->vertexShader = vertexShader;
		ptr->pixelShaderCB = pixelShaderCB->Clone();
		ptr->pixelShader = pixelShader;

		Materials.StoreAsset(cloneName, ptr);
		return ptr;
	}

	void Material::OnImgui(bool assetInfo)
	{
		if (ImGui::TreeNodeEx("Material", ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_DefaultOpen, "Material %s", name.c_str()))
		{
			if (ImGui::BeginPopupContextItem())
			{
				if (ImGui::MenuItem("Clone")) Clone();
				ImGui::EndPopup();
			}

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
