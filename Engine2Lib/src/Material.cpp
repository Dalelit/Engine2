#include "pch.h"
#include "Material.h"

namespace Engine2
{
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
		if (texture) texture->Bind();
	}

	void Material::ShadowBind()
	{
		vertexShaderCB->Bind();
	}

	void Material::SetTransform(TransformMatrix& transform)
	{
		vertexShaderCB->data = transform;
		vertexShaderCB->UpdateBuffer();
	}

	std::shared_ptr<Material> Material::Clone(const std::string& cloneName)
	{
		auto ptr = std::make_shared<Material>(cloneName);

		ptr->vertexShaderCB = std::make_shared<VSConstantBuffer<StandardVSData>>(*vertexShaderCB); // vertexShaderCB->Clone();
		ptr->vertexShader = vertexShader;
		ptr->pixelShaderCB = pixelShaderCB->Clone();
		ptr->pixelShader = pixelShader;
		ptr->texture = texture;

		return ptr;
	}

	void Material::OnImgui()
	{
		if (ImGui::TreeNodeEx("Material", ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_DefaultOpen, "Material %s", name.c_str()))
		{
			//if (ImGui::BeginPopupContextItem())
			//{
			//	if (ImGui::MenuItem("Clone")) Clone();
			//	ImGui::EndPopup();
			//}

			if (vertexShaderCB) vertexShaderCB->OnImgui();
			else ImGui::Text("vertexShaderCB null");

			if (vertexShader) vertexShader->OnImgui();
			else ImGui::Text("vertexShader null");

			if (pixelShaderCB) pixelShaderCB->OnImgui();
			else ImGui::Text("pixelShaderCB null");

			if (pixelShader) pixelShader->OnImgui();
			else ImGui::Text("pixelShader null");

			if (texture) texture->OnImgui();
			else ImGui::Text("texture null");

			ImGui::TreePop();
		}
	}
}
