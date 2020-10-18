#include "pch.h"
#include "Mesh.h"

namespace Engine2
{
	void Mesh::OnImgui()
	{
		if (ImGui::TreeNodeEx("Mesh", ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_DefaultOpen))
		{
			if (drawable) drawable->OnImgui();
			else ImGui::Text("Drawable null");

			if (vertexShaderCB) vertexShaderCB->OnImgui();
			else ImGui::Text("vertexShaderCB null");

			if (vertexShader) vertexShader->OnImgui();
			else ImGui::Text("vertexShader null");

			if (pixelShader) pixelShader->OnImgui();
			else ImGui::Text("pixelShader null");

			ImGui::TreePop();
		}
	}
}