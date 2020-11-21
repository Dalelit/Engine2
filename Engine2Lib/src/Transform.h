#pragma once
#include <d3d11.h>

namespace Engine2
{
	struct Transform
	{
		DirectX::XMVECTOR position = { 0.0f, 0.0f, 0.0f, 1.0f };
		DirectX::XMVECTOR scale = { 1.0f, 1.0f, 1.0f, 0.0f };
		DirectX::XMVECTOR rotation = { 0.0f, 0.0f, 0.0f, 0.0f };

		DirectX::XMMATRIX Matrix();
		inline DirectX::XMMATRIX MatrixTransposed() { return DirectX::XMMatrixTranspose(Matrix()); }

		void SetPosition(float x, float y, float z) { position = { x, y, z, 1.0f }; }
		void SetRotation(float x, float y, float z) { rotation = { x, y, z, 0.0f }; }
		void SetScale(float x, float y, float z)    { scale = { x, y, z, 0.0f }; }
		void SetScale(float s)                      { scale = { s, s, s, 0.0f }; }

		// Note: yaw, ptich only. Roll is locked
		void LookAt(DirectX::XMVECTOR location);
		inline void LookAt(float x, float y, float z) { LookAt({ x, y, z, 1.0f }); }

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
			rotationMatrix = DirectX::XMMatrixTranspose(source.rotationMatrix);
		}

		//inline void Transpose() {
		//	transformMatrix = DirectX::XMMatrixTranspose(transformMatrix);
		//	rotationMatrix = DirectX::XMMatrixTranspose(rotationMatrix);
		//}

		inline DirectX::XMVECTOR GetTranslation() { return DirectX::XMMatrixTranspose(transformMatrix).r[3]; }

		inline DirectX::XMMATRIX MatrixTransposed() { return DirectX::XMMatrixTranspose(transformMatrix); }

		void OnImgui();
	};
}
