#include "pch.h"
#include "Lights.h"

namespace Engine2
{
	void PointLight::OnImgui()
	{
		ImGui::ColorEdit3("Color", color.m128_f32);
	}
}