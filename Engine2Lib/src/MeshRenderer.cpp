#include "pch.h"
#include "MeshRenderer.h"
#include "AssetManager.h"

namespace Engine2
{
	std::weak_ptr<Mesh> MeshRenderer::defaultMesh;
	std::weak_ptr<Material> MeshRenderer::defaultMaterial;

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
		if (newMesh) mesh = newMesh;

		ImGui::SameLine();
		if (mesh) mesh->OnImgui();
		else ImGui::Text("Mesh null");


		if (ImGui::Button("Mat")) AssetManager::Manager().OnImguiSelectMaterialPopupOpen();
		auto newMat = AssetManager::Manager().OnImguiSelectMaterial();
		if (newMat) material = newMat;

		ImGui::SameLine();
		if (material) material->OnImgui();
		else ImGui::Text("Material null");
	}
}
