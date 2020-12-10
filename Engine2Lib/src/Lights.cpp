#include "pch.h"
#include "Lights.h"
#include "UtilMath.h"

namespace Engine2
{
	using namespace DirectX;

	//*************************

	void PointLight::OnImgui()
	{
		ImGui::ColorEdit3("Color", color.m128_f32);
	}

	//*************************

	DirectionalLight::DirectionalLight() : camera("Shadow camera")
	{
		SetRotation(-XM_PIDIV4, -XM_PI * 0.75f, 0.0f);

		camera.SetOrthographic();
		camera.SetAspectRatio(1.0f);
		
	}

	void DirectionalLight::OnImgui()
	{
		ImGui::ColorEdit3("Color", color.m128_f32);

		XMVECTOR degs = Math::RadToDeg(rotation);
		if (ImGui::DragFloat3("Angle", degs.m128_f32, 0.5f)) SetRotation(Math::DegToRad(degs));
		ImGui::DragFloat3("Position", position.m128_f32, 0.5f);
		ImGui::Checkbox("Cast shadows", &castShadows);
		camera.OnImgui();
	}
}