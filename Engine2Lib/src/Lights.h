#pragma once
#include "pch.h"
#include "Common.h"
#include "Camera.h"
#include "Shader.h"
#include "ConstantBuffer.h"
#include "ShadowMap.h"

namespace Engine2
{
	class PointLight
	{
	public:
		PointLight() = default;
		PointLight(DirectX::XMVECTOR color) : color(color) {}

		void OnImgui();

		DirectX::XMVECTOR color = { 1.0f, 1.0f, 1.0f, 1.0f};
	};

	class DirectionalLight
	{
	public:
		DirectionalLight();

		void OnImgui();

		inline void SetRotation(float x, float y, float z) { SetRotation({x, y, z, 1.0f}); }
		inline void SetRotation(DirectX::XMVECTOR rot) { rotation = rot; CalcPosition(); }

		inline Camera& GetCamera() { return camera; }

		DirectX::XMMATRIX& GetTransform() { return transform; }
		DirectX::XMMATRIX GetTransformT() { return DirectX::XMMatrixTranspose(GetTransform()); }

		bool ShowGizmos() { return showGizmos; }

		void ShadowPassStart();
		void ShadowPassEnd();
		void BindShadowMap();

	protected:
		DirectX::XMVECTOR rotation;
		DirectX::XMMATRIX transform;

		DirectX::XMVECTOR centre = DirectX::g_XMZero;
		DirectX::XMVECTOR position;

		Camera camera;
		ShadowMap shadowMap;
		std::unique_ptr<VertexShaderFile> pVSShader;

		UINT32 shadowMapSlot = 1u;
		struct ShadowPSCBData
		{
			DirectX::XMMATRIX viewProjection;
			DirectX::XMVECTOR lightColor;
			DirectX::XMVECTOR lightDirection;
			float shadowBias;
		};
		PSConstantBuffer<ShadowPSCBData> pscbShadowCamera;

		bool showGizmos = true;

		void CalcPosition();
	};
}