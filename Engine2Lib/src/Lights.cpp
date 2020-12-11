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
		SetRotation(Math::DegToRad({-45.0f, 135.0f, 0.0f, 0.0f}));
		XMVECTOR degs = Math::RadToDeg(rotation);

		camera.SetOrthographic();
		camera.SetAspectRatio(1.5f);

		CalcPosition();
	}

	void DirectionalLight::OnImgui()
	{
		ImGui::ColorEdit3("Color", color.m128_f32);

		XMVECTOR degs = Math::RadToDeg(rotation);
		if (ImGui::DragFloat3("Angle", degs.m128_f32, 0.5f)) SetRotation(Math::DegToRad(degs));
		if (ImGui::DragFloat3("Centre", centre.m128_f32, 0.5f)) CalcPosition();
		ImGui::DragFloat3("Position", position.m128_f32, 0.5f);
		ImGui::Checkbox("Show gizmos", &showGizmos);
		camera.OnImgui();
	}

	DirectX::XMMATRIX DirectionalLight::GetModelViewProjectionMatrixT()
	{
		XMMATRIX m;
		camera.LoadViewProjectionMatrixT(m);
		return m;
	}

	void DirectionalLight::CalcPosition()
	{
		auto rotq = XMQuaternionRotationRollPitchYawFromVector(rotation);

		float distance = camera.FarPlane() - camera.NearPlane();

		position = centre - XMVector3Rotate({0.0f, 0.0f, 1.0f, 1.0f}, rotq) * distance * 0.5;

		transform = XMMatrixRotationRollPitchYawFromVector(rotation) * XMMatrixTranslationFromVector(position);

		camera.Update(position, rotation);
	}
}