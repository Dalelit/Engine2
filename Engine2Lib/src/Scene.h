#pragma once
#include "pch.h"
#include "Camera.h"
#include "ConstantBuffer.h"

namespace Engine2
{
	// Used for scene level resources that are commonly used and bound once per frame.
	class Scene
	{
	public:

		~Scene() = default;

		void OnUpdate(float dt);
		void OnRender();
		void OnImgui();

		Camera mainCamera;

		struct VSSceneData
		{
			DirectX::XMMATRIX cameraTransform;
		};

		static std::string GetVSCBHLSL() { return R"(
			cbuffer sceneConst
			{
				matrix cameraTransform;
			};
		)"; }

		VSConstantBuffer<VSSceneData> vsConstBuffer;

	protected:
	};

}

