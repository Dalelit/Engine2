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

	DirectionalLight::DirectionalLight() :
		camera("Shadow camera"), pscbShadowCamera(2u)
	{
		SetRotation(Math::DegToRad({30.0f, 40.0f, 0.0f, 0.0f}));
		XMVECTOR degs = Math::RadToDeg(rotation);

		camera.SetOrthographic();
		camera.SetAspectRatio(1.5f);
		camera.SetNearPlane(0.1f);
		camera.SetFarPlane(30.0f);

		shadowMap.Initialise(2048, 2048);

		pscbShadowCamera.data.lightColor = { 0.5f, 0.5f, 0.5f, 1.0f };
		pscbShadowCamera.data.shadowBias = 0.005f;

		// to do: these may need to move into the various components so they can have their own versions of a shadow vs. Currently relies on name conventions

		std::vector<D3D11_INPUT_ELEMENT_DESC> vsLayout = {
			{"Position", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
		};

		vsShader.CompileFromFile(Config::directories["EngineShaderSourceDir"] + "PositionShadowVS.hlsl", vsLayout, "main");

		std::vector<D3D11_INPUT_ELEMENT_DESC> vsInstancedLayout = {
			{"Position", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"InstanceTransform", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{"InstanceTransform", 1, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{"InstanceTransform", 2, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{"InstanceTransform", 3, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA, 1},
		};

		vsShaderInstanced.CompileFromFile(Config::directories["EngineShaderSourceDir"] + "PositionShadowVS.hlsl", vsInstancedLayout, "mainInstanced");
	}

	void DirectionalLight::OnImgui()
	{
		ImGui::ColorEdit3("Color", pscbShadowCamera.data.lightColor.m128_f32);
		ImGui::DragFloat("Shadow bias", &pscbShadowCamera.data.shadowBias, 0.0005f);

		XMVECTOR degs = Math::RadToDeg(rotation);
		if (ImGui::DragFloat3("Angle", degs.m128_f32, 0.5f)) SetRotation(Math::DegToRad(degs));
		ImGui::DragFloat3("Centre", centre.m128_f32, 0.5f);
		ImGui::DragFloat3("Position", position.m128_f32, 0.5f);
		camera.OnImgui();
		vsShader.OnImgui();
		shadowMap.OnImgui();
	}

	void DirectionalLight::ShadowPassStart(WorldCamera viewCamera)
	{
		CalcPosition(viewCamera);
		auto& device = DXDevice::Get();
		device.ClearShaderResource(shadowMapSlot); //offscreen.Unbind();
		device.SetNoFaceCullingRenderState();
		shadowMap.Clear();
		shadowMap.SetAsTarget();
		vsShader.Bind();
	}

	void DirectionalLight::ShadowPassEnd()
	{
		auto& device = DXDevice::Get();
		device.SetBackBufferAsRenderTarget();
		device.SetDefaultRenderState();
		device.SetDefaultViewport();
	}

	void DirectionalLight::BindShadowMap()
	{
		shadowMap.BindWtihComparisonSampler();

		camera.LoadViewProjectionMatrixT(pscbShadowCamera.data.viewProjection);
		pscbShadowCamera.Bind();
	}

	void DirectionalLight::CalcPosition(WorldCamera viewCamera)
	{
		// get the light direction based on the rotation
		auto rotq = XMQuaternionRotationRollPitchYawFromVector(rotation);
		pscbShadowCamera.data.lightDirection = XMVector3Rotate({ 0.0f, 0.0f, 1.0f, 1.0f }, rotq);
		

		// convert the bounds of the view camera frustrum into the light's coordinates so the bounds can be worked out
		auto lightWorldTransform = XMMatrixRotationQuaternion(rotq);
		auto mInv = XMMatrixInverse(nullptr, lightWorldTransform); // to convert world points to light positions
		auto points = viewCamera.GetFrustrumPoints(); // points in world space
		std::for_each(points.begin(), points.end(), [&](auto& p) { p = XMVector4Transform(p, mInv); }); // now points in light coordinates

		// use a bounding box to get the extents of the light
		BoundingBox bounds;
		BoundingBox::CreateFromPoints(bounds, points.size(), (XMFLOAT3*)points.data(), sizeof(XMVECTOR));

		// update the camera size to fit the bounds of the view camera frustrum
		camera.SetViewWidth(bounds.Extents.x * 2.0f);
		camera.SetViewHeight(bounds.Extents.y * 2.0f);

		// set the near plane to a fixed distance, make the far plan cover the full distance of the points
		constexpr float nearDist = 0.5f;
		float distance = bounds.Extents.z * 2.0f;
		camera.SetNearPlane(nearDist);
		camera.SetFarPlane(distance + nearDist);

		// set the shadow camera position - from the centre of the view camera frustrum, move back in the direction of the light
		centre = viewCamera.GetFrustrumCentre();
		position = centre - pscbShadowCamera.data.lightDirection * (bounds.Extents.z + nearDist);

		// set the transform which will be passed to the shaders
		//transform = lightWorldTransform * XMMatrixTranslationFromVector(position);

		// update the shadow camera
		camera.Update(position, rotation);
	}
}