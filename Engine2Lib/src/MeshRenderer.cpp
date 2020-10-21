#include "pch.h"
#include "MeshRenderer.h"

namespace Engine2
{
	void MeshRenderer::BindAndDraw()
	{
		material->Bind();
		mesh->BindAndDraw();
	}

	void MeshRenderer::OnImgui()
	{
		if (ImGui::TreeNodeEx("MeshRenderer", ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_DefaultOpen))
		{
			if (mesh) mesh->OnImgui();
			else ImGui::Text("Mesh null");
			auto newMesh = Mesh::Assets.OnImguiSelector();
			if (newMesh) mesh = newMesh;

			if (material) material->OnImgui();
			else ImGui::Text("Material null");
			auto newMat = Material::Assets.OnImguiSelector();
			if (newMat) material = newMat;

			ImGui::TreePop();
		}
	}
}
