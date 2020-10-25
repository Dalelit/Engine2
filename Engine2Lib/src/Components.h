#pragma once

#include <string>
#include "ECS.h"

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
		
		void Set(float positionX = 0.0f, float positionY = 0.0f, float positionZ = 0.0f,
				 float scaleX = 1.0f, float scaleY = 1.0f, float scaleZ = 1.0f,
				 float rollDeg = 0.0f, float pitchDeg = 0.0f, float yawDeg = 0.0f);

		inline DirectX::XMMATRIX GetTransformTranspose() { return DirectX::XMMatrixTranspose(transform); }

		inline DirectX::XMVECTOR GetTranslation() { return DirectX::XMMatrixTranspose(transform).r[3]; }

		inline Transform& Multiply(DirectX::XMMATRIX m) { transform = DirectX::XMMatrixMultiplyTranspose(DirectX::XMMatrixTranspose(transform), m); return *this; }
		
		void OnImgui();
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