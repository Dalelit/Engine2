#include "pch.h"
#include "Lights.h"
#include "UtilMath.h"
#include "VertexLayout.h"

namespace Engine2
{
	using namespace DirectX;

	//*************************

	void PointLight::OnImgui()
	{
		ImGui::ColorEdit3("Color", color.m128_f32);
	}

	//*************************

	DirectionalLight::DirectionalLight() :
		camera("Shadow camera"), offscreen(false, true), pscbShadowCamera(2u)
	{
		SetRotation(Math::DegToRad({45.0f, 135.0f, 0.0f, 0.0f}));
		XMVECTOR degs = Math::RadToDeg(rotation);

		camera.SetOrthographic();
		camera.SetAspectRatio(1.5f);
		camera.SetNearPlane(0.1f);
		camera.SetFarPlane(30.0f);

		pscbShadowCamera.data.lightColor = { 0.5f, 0.5f, 0.5f, 1.0f };
		pscbShadowCamera.data.shadowBias = 0.005f;

		pVSShader = std::make_unique<VertexShaderFile>(Config::directories["EngineShaderSourceDir"] + "PositionShadowVS.hlsl", VertexLayout::Position::Layout);
	}

	void DirectionalLight::OnImgui()
	{
		ImGui::ColorEdit3("Color", pscbShadowCamera.data.lightColor.m128_f32);
		ImGui::DragFloat("Shadow bias", &pscbShadowCamera.data.shadowBias, 0.0005f);

		XMVECTOR degs = Math::RadToDeg(rotation);
		if (ImGui::DragFloat3("Angle", degs.m128_f32, 0.5f)) SetRotation(Math::DegToRad(degs));
		ImGui::DragFloat3("Centre", centre.m128_f32, 0.5f);
		ImGui::DragFloat3("Position", position.m128_f32, 0.5f);
		ImGui::Checkbox("Show gizmos", &showGizmos);
		camera.OnImgui();
		offscreen.OnImgui();
		pVSShader->OnImgui();
	}

	DirectX::XMMATRIX DirectionalLight::GetModelViewProjectionMatrixT()
	{
		XMMATRIX m;
		camera.LoadViewProjectionMatrixT(m);
		return m;
	}

	void DirectionalLight::ShadowPassStart()
	{
		CalcPosition();
		DXDevice::Get().ClearShaderResource(shadowMapSlot); //offscreen.Unbind();
		offscreen.Clear();
		offscreen.SetAsTarget();
		pVSShader->Bind();
	}

	void DirectionalLight::ShadowPassEnd()
	{
		DXDevice::Get().SetBackBufferAsRenderTarget();
	}

	void DirectionalLight::BindShadowMap()
	{
		offscreen.BindDepthBuffer(shadowMapSlot);

		camera.LoadViewProjectionMatrixT(pscbShadowCamera.data.viewProjection);
		pscbShadowCamera.Bind();
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