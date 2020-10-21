#include "pch.h"
#include "Mesh.h"

namespace Engine2
{
	AssetStore<Mesh> Mesh::Assets;

	void Mesh::BindAndDraw()
	{
		drawable->BindAndDraw();
	}

	void Mesh::OnImgui(bool assetInfo)
	{
		if (ImGui::TreeNodeEx("Mesh", ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_DefaultOpen, "Mesh %s", name.c_str()))
		{
			if (assetInfo) ImGui::Text("Drawable references %i", drawable.use_count());

			if (drawable) drawable->OnImgui();
			else ImGui::Text("Drawable null");

			ImGui::TreePop();
		}
	}
}