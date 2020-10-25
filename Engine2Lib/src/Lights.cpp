#include "pch.h"
#include "Lights.h"

namespace Engine2
{
	void PointLight::OnImgui()
	{
		if (ImGui::TreeNode("PointLight"))
		{
			ImGui::ColorEdit3("Color", color.m128_f32);
			ImGui::TreePop();
		}
	}
}