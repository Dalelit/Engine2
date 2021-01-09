#include "pch.h"
#include "MeshRenderer.h"

namespace Engine2
{
	std::shared_ptr<Mesh> MeshRenderer::defaultMesh;
	std::shared_ptr<Material> MeshRenderer::defaultMaterial;

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
		if (mesh) mesh->OnImgui();
		else ImGui::Text("Mesh null");
		auto newMesh = Mesh::Assets.OnImguiSelector("Meshes");
		if (newMesh) mesh = newMesh;

		if (material) material->OnImgui();
		else ImGui::Text("Material null");
		auto newMat = Material::Materials.OnImguiSelector("Materials");
		if (newMat) material = newMat;
	}
}
