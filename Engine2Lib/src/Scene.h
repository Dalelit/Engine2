#pragma once
#include "pch.h"
#include "ConstantBuffer.h"

namespace Engine2
{
	// Used for scene level resources that are commonly used and bound once per frame.
	class Scene
	{
	public:

		Scene() : vsConstBuffer(0) {}
		~Scene() = default;

		void OnUpdate(float dt);
		void OnRender();

		struct VSSceneData
		{
			DirectX::XMMATRIX cameraTransform;
		};

		static std::string GetVSCBHLSL() { return R"(
			cbuffer sceneConst : register (b0)
			{
				matrix cameraTransform;
			};
		)"; }

		VSConstantBuffer<VSSceneData> vsConstBuffer;

	protected:
	};

}

