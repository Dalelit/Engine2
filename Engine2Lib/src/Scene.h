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

		std::vector<PointLight> pointLights;

		struct PSSceneData
		{
			DirectX::XMVECTOR CameraPosition;
			DirectX::XMVECTOR ambientLight;
			DirectX::XMVECTOR pointLightPosition;
			DirectX::XMVECTOR pointLightColor;
		};
		PSConstantBuffer<PSSceneData> psConstBuffer;

	protected:
		void UpdateVSConstBuffer();
		void UpdatePSConstBuffer();
	};

}

