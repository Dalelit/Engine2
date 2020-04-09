#include "pch.h"
#include "Lights.h"

namespace Engine2
{
	void PointLight::OnImgui()
	{
		if (ImGui::TreeNode(id.c_str()))
		{
			ImGui::Checkbox("Active", &active);
			ImGui::DragFloat3("Position", position.m128_f32, 0.1f);
			ImGui::ColorEdit3("Color", color.m128_f32);
			ImGui::TreePop();
		}
	}
}