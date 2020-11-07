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
		DirectX::XMVECTOR position = { 0.0f, 0.0f, 0.0f, 1.0f };
		DirectX::XMVECTOR scale = { 1.0f, 1.0f, 1.0f, 0.0f };
		DirectX::XMVECTOR rotation = { 0.0f, 0.0f, 0.0f, 0.0f };
		
		DirectX::XMMATRIX Matrix();
		inline DirectX::XMMATRIX MatrixTransposed() { return DirectX::XMMatrixTranspose(Matrix()); }

		void OnImgui();
	};

	struct TransformMatrix
	{
		DirectX::XMMATRIX matrix;

		inline DirectX::XMVECTOR GetTranslation() { return DirectX::XMMatrixTranspose(matrix).r[3]; }

		inline DirectX::XMMATRIX MatrixTransposed() { return DirectX::XMMatrixTranspose(matrix); }

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