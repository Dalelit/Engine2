#include "pch.h"
#include "Skyscape.h"

void Skyscape::OnImgui()
{
	if (ImGui::TreeNode("Skyscape"))
	{
		Skybox::OnImgui();
		ImGui::TreePop();
	}
}

void Skyscape::SetPS()
{
	pixelShader.CompileFromFile("src\\SceneBuidler\\SkyboxPS.hlsl");
}
