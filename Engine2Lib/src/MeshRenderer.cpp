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

	void MeshRenderer::OnImgui()
	{
		if (mesh) mesh->OnImgui();
		else ImGui::Text("Mesh null");
		auto newMesh = Mesh::Assets.OnImguiSelector();
		if (newMesh) mesh = newMesh;

		if (material) material->OnImgui();
		else ImGui::Text("Material null");
		auto newMat = Material::Materials.OnImguiSelector();
		if (newMat) material = newMat;
	}
}
