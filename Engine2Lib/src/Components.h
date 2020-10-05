#pragma once

#include <string>
#include "submodules/imgui/imgui.h"
#include "ECS.h"
#include "Resources.h"
#include "ConstantBuffer.h"
#include "Shader.h"
#include "Particles.h"

namespace Engine2
{
	struct EntityInfo
	{
		std::string tag;

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

		bool IsValid() { return drawable && vertexShader && vertexShaderCB && pixelShader; }
	};

	struct Gizmo
	{
		enum Types {Axis, Cube, Sphere, Camera};
		Types type = Types::Sphere;

		void OnImgui();
	};

	class Components
	{
	public:
		static void OnImgui(EngineECS::EntityId_t id, EngineECS::Coordinator& coord);
	};
}