#include "pch.h"
#include "Mesh.h"

namespace Engine2
{
	AssetStore<Mesh> Mesh::Assets;

	void Mesh::OnImgui()
	{
		if (ImGui::TreeNodeEx("Mesh", ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_DefaultOpen, name.c_str()))
		{
			if (drawable) drawable->OnImgui();
			else ImGui::Text("Drawable null");

			ImGui::TreePop();
		}
	}

}