#include "pch.h"
#include "Lights.h"
#include "UtilMath.h"

namespace Engine2
{
	using namespace DirectX;

	void PointLight::OnImgui()
	{
		ImGui::ColorEdit3("Color", color.m128_f32);
	}

	DirectionalLight::DirectionalLight()
	{
		SetRotation(XM_PIDIV4, XM_PIDIV4, 0.0f);
	}

	void DirectionalLight::OnImgui()
	{
		ImGui::ColorEdit3("Color", color.m128_f32);

		XMVECTOR degs = Math::RadToDeg(rotation);
		if (ImGui::DragFloat3("Angle", degs.m128_f32, 0.5f)) SetRotation(Math::DegToRad(degs));

		ImGui::Checkbox("Cast shadows", &castShadows);
	}
}