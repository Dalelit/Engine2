#pragma once

#include <string>
#include "submodules/imgui/imgui.h"
#include "ECS.h"
#include "Resources.h"
#include "ConstantBuffer.h"
#include "Shader.h"

namespace Engine2
{
	struct EntityInfo
	{
		std::string tag; // To Do: a better name?

		void OnImgui();
	};

	struct Transform
	{
		DirectX::XMMATRIX transform = { { 1.0f, 0.0f, 0.0f, 0.0f },
										{ 0.0f, 1.0f, 0.0f, 0.0f },
										{ 0.0f, 0.0f, 1.0f, 0.0f },
										{ 0.0f, 0.0f, 0.0f, 1.0f } };
		
		// To do: remove this once I work out how to get degree rotation out of the transform
		struct {
			float roll = 0.0f, pitch = 0.0f, yaw = 0.0f;
		} rotation; // degrees for display purposes

		void Set(float positionX, float positionY, float positionZ, float scaleX, float scaleY, float scaleZ, float rollDeg, float pitchDeg, float yawDeg);

		inline Transform& Multiply(DirectX::XMMATRIX m) { transform = DirectX::XMMatrixMultiplyTranspose(DirectX::XMMatrixTranspose(transform), m); return *this; }
		
		void OnImgui();
	};

	struct Mesh
	{
		std::shared_ptr<Drawable> drawable;
		std::shared_ptr<VSConstantBuffer<Transform>> vertexShaderCB;
		std::shared_ptr<VertexShader> vertexShader;
		std::shared_ptr<PixelShader> pixelShader;

		void OnImgui();
	};

	class Components
	{
	public:
		static void OnImgui(EngineECS::EntityId_t id, EngineECS::Coordinator& coord)
		{
			if (coord.HasComponent<EntityInfo>(id)) coord.GetComponent<EntityInfo>(id)->OnImgui();
			if (coord.HasComponent<Transform>(id)) coord.GetComponent<Transform>(id)->OnImgui();
			if (coord.HasComponent<Mesh>(id)) coord.GetComponent<Mesh>(id)->OnImgui();
		}
	};
}