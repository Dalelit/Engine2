#include "pch.h"
#include "MeshRenderer.h"

namespace Engine2
{
	void MeshRenderer::BindAndDraw()
	{
		material->PreDraw();
		material->Bind();
		mesh->BindAndDraw();
		material->PostDraw();
	}

	void MeshRenderer::ShadowBindAndDraw()
	{
		//material->PreDraw();

		material->ShadowBind();

		mesh->BindAndDraw();

		//material->PostDraw();
	}

	void MeshRenderer::OnImgui()
	{
		if (ImGui::Button("Mesh")) AssetManager::Manager().OnImguiSelectMeshPopupOpen();
		auto newMesh = AssetManager::Manager().OnImguiSelectMesh();
		if (newMesh.first) meshAsset = newMesh.first;
		if (newMesh.second) mesh = newMesh.second;

		ImGui::SameLine();
		if (mesh) mesh->OnImgui();
		else ImGui::Text("Mesh null");


		if (ImGui::Button("Mat")) AssetManager::Manager().OnImguiSelectMaterialPopupOpen();
		auto newMat = AssetManager::Manager().OnImguiSelectMaterial();
		if (newMat.first) materialAsset = newMat.first;
		if (newMat.second) material = newMat.second;

		ImGui::SameLine();
		if (material) material->OnImgui();
		else ImGui::Text("Material null");
	}

	void MeshRenderer::Serialise(Serialisation::INode& node)
	{
		if (node.Saving())
		{
			node.Attribute("meshAsset", (meshAsset ? meshAsset->GetSource() : ""));
			node.Attribute("mesh", (mesh ? mesh->Name() : ""));
			node.Attribute("materialAsset", (materialAsset ? materialAsset->GetSource() : ""));
			node.Attribute("material", (material ? material->Name() : ""));
		}
		else
		{
			std::string meshAssetName, meshName, materialAssetName, materialName;

			node.Attribute("meshAsset", meshAssetName);
			node.Attribute("mesh", meshName);
			node.Attribute("materialAsset", materialAssetName);
			node.Attribute("material", materialName);

			// load mesh
			{
				auto ma = AssetManager::Manager().FindAsset(meshAssetName);
				if (ma)
				{
					meshAsset = &ma.value().get();
					mesh = ma.value().get().Meshes().GetAsset(meshName);
				}
			}

			// load material
			{
				auto ma = AssetManager::Manager().FindAsset(materialAssetName);
				if (ma)
				{
					materialAsset = &ma.value().get();
					material = ma.value().get().Materials().GetAsset(materialName);
				}
			}

		}
	}
}
