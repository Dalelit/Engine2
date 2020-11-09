#include "pch.h"
#include "Material.h"

namespace Engine2
{
	AssetStore<Material>         Material::Materials;
	AssetStore<VertexShaderFile> Material::VertexShaders;
	AssetStore<PixelShaderFile>  Material::PixelShaders;

	std::shared_ptr<VertexShaderFile> Material::GetVertexShader(const std::string& filename, VertexShaderLayoutDesc& layout)
	{
		// retrieve it if already loaded
		auto existing = Material::VertexShaders.GetAsset(filename);
		if (existing) return existing;

		// create
		auto ptr = std::make_shared<VertexShaderFile>(filename, layout);

		if (!ptr) return nullptr; // failed to load

		// add to the store
		Material::VertexShaders.StoreAsset(filename, ptr);

		return ptr;
	}

	std::shared_ptr<PixelShaderFile> Material::GetPixelShader(const std::string& filename)
	{
		// retrieve it if already loaded
		auto existing = Material::PixelShaders.GetAsset(filename);
		if (existing) return existing;

		// create
		auto ptr = std::make_shared<PixelShaderFile>(filename);

		if (!ptr) return nullptr; // failed to load

		// add to the store
		Material::PixelShaders.StoreAsset(filename, ptr);

		return ptr;
	}

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
