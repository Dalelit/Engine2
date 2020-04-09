#pragma once
#include "pch.h"
#include "ConstantBuffer.h"
#include "Lights.h"

namespace Engine2
{
	// Used for scene level resources that are commonly used and bound once per frame.
	class Scene
	{
	public:

		Scene();
		~Scene() = default;

		void OnUpdate(float dt) {}
		void OnRender();

		void OnImgui();

		struct VSSceneData
		{
			DirectX::XMMATRIX cameraTransform;
		};
		VSConstantBuffer<VSSceneData> vsConstBuffer;

		struct PSSceneData
		{
			DirectX::XMVECTOR CameraPosition;
			DirectX::XMVECTOR pointLightPosition;
			DirectX::XMVECTOR pointLightColor;
		};
		PSConstantBuffer<PSSceneData> psConstBuffer;

		std::vector<PointLight> pointLights;


		static std::string GetVSCBHLSL() {
			return R"(
			cbuffer sceneConst : register (b0)
			{
				matrix cameraTransform;
			};
		)";
		}

	protected:
		void UpdateVSConstBuffer();
		void UpdatePSConstBuffer();
	};

}

