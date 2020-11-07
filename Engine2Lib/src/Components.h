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
		// note: order is important for the constructor based on transform setting the rot matrix first then using it
		DirectX::XMMATRIX rotationMatrix;
		DirectX::XMMATRIX transformMatrix;

		TransformMatrix() = default;
		TransformMatrix(Transform& transform);
		TransformMatrix(DirectX::XMMATRIX transformMatrix, DirectX::XMMATRIX rotationMatrix);

		void Set(Transform& transform);

		//inline void SetTranspose(Transform& transform) { Set(transform); Transpose(); }

		inline void SetTranspose(TransformMatrix& source) {
			transformMatrix = DirectX::XMMatrixTranspose(source.transformMatrix);
			rotationMatrix  = DirectX::XMMatrixTranspose(source.rotationMatrix);
		}

		//inline void Transpose() {
		//	transformMatrix = DirectX::XMMatrixTranspose(transformMatrix);
		//	rotationMatrix = DirectX::XMMatrixTranspose(rotationMatrix);
		//}

		inline DirectX::XMVECTOR GetTranslation() { return DirectX::XMMatrixTranspose(transformMatrix).r[3]; }

		inline DirectX::XMMATRIX MatrixTransposed() { return DirectX::XMMatrixTranspose(transformMatrix); }

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