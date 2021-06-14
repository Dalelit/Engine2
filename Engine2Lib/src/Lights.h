#pragma once
#include "pch.h"
#include "Common.h"
#include "Camera.h"
#include "Shader.h"
#include "ConstantBuffer.h"
#include "ShadowMap.h"
#include "Serialiser.h"

namespace Engine2
{
	class PointLight
	{
	public:
		PointLight() = default;
		PointLight(DirectX::XMVECTOR color) : color(color) {}

		void OnImgui();

		void Serialise(Serialisation::INode& node) { node.Attribute("color", color); }

		DirectX::XMVECTOR color = { 1.0f, 1.0f, 1.0f, 1.0f};
	};

	class DirectionalLight
	{
	public:
		DirectionalLight();

		void OnImgui();

		inline void SetRotation(float x, float y, float z) { SetRotation({x, y, z, 1.0f}); }
		inline void SetRotation(DirectX::XMVECTOR rot) { rotation = rot; }

		inline Camera& GetCamera() { return camera; }

		void ShadowPassStart(WorldCamera viewCamera);
		void ShadowPassEnd();
		void BindShadowMap();

		void BindShadowVS() { vsShader.Bind(); }
		void BindShadowVSInstanced() { vsShaderInstanced.Bind(); }

		DirectX::XMVECTOR GetCentre() { return centre; }

		DirectX::XMVECTOR GetDireciton() { return pscbShadowCamera.data.lightDirection; }
		DirectX::XMVECTOR GetColor()     { return pscbShadowCamera.data.lightColor; }

	protected:
		DirectX::XMVECTOR rotation;
		DirectX::XMVECTOR centre;
		DirectX::XMVECTOR position;

		Camera camera;
		ShadowMap shadowMap;
		VertexShader vsShader;
		VertexShader vsShaderInstanced;

		UINT32 shadowMapSlot = 1u;
		struct ShadowPSCBData
		{
			DirectX::XMMATRIX viewProjection;
			DirectX::XMVECTOR lightColor;
			DirectX::XMVECTOR lightDirection;
			float shadowBias;
		};
		PSConstantBuffer<ShadowPSCBData> pscbShadowCamera;

		void CalcPosition(WorldCamera viewCamera);
	};
}