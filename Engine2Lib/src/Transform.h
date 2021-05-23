#pragma once
#include <d3d11.h>
#include "Serialiser.h"

namespace Engine2
{
	class Transform
	{
	public:
		DirectX::XMVECTOR position = { 0.0f, 0.0f, 0.0f, 1.0f };
		DirectX::XMVECTOR scale = { 1.0f, 1.0f, 1.0f, 0.0f };
		DirectX::XMVECTOR rotation = { 0.0f, 0.0f, 0.0f, 0.0f };

		DirectX::XMMATRIX Matrix();
		inline DirectX::XMMATRIX MatrixTransposed() { return DirectX::XMMatrixTranspose(Matrix()); }

		DirectX::XMVECTOR Forward();

		void SetPosition(float x, float y, float z) { position = { x, y, z, 1.0f }; }
		void SetRotation(float x, float y, float z) { rotation = { x, y, z, 0.0f }; }
		void SetScale(float x, float y, float z)    { scale = { x, y, z, 0.0f }; }
		void SetScale(float s)                      { scale = { s, s, s, 0.0f }; }

		// Note: yaw, ptich only. Roll is locked
		void LookAt(DirectX::XMVECTOR location);
		inline void LookAt(float x, float y, float z) { LookAt({ x, y, z, 1.0f }); }

		void Move(float forward);
		void Move(float forward, float right, float up);

		inline void Translate(float x, float y, float z) { Translate({ x, y, z, 0.0f }); }
		inline void Translate(DirectX::XMVECTOR vector) { position = DirectX::XMVectorAdd(position, vector); } // position += vector

		inline void Rotate(float pitch, float yaw, float roll) { Rotate({pitch, yaw, roll, 0.0f}); }
		inline void Rotate(DirectX::XMVECTOR vector) { rotation = DirectX::XMVectorAdd(rotation, vector); } // rotation += vector

		inline void Scale(float s) { Scale({ s, s, s, 1.0f }); }
		inline void Scale(float x, float y, float z) { Scale({ x, y, z, 1.0f }); }
		inline void Scale(DirectX::XMVECTOR vector) { scale = DirectX::XMVectorMultiply(scale, vector); } // scale *= vector

		void OnImgui();

		void Serialise(Serialisation::INode& node);

		static struct {
			static constexpr DirectX::XMVECTOR up = { 0.0f, 1.0f, 0.0f, 0.0f };
			static constexpr DirectX::XMVECTOR down = { 0.0f, -1.0f, 0.0f, 0.0f };
			static constexpr DirectX::XMVECTOR right = { 1.0f, 0.0f, 0.0f, 0.0f };
			static constexpr DirectX::XMVECTOR left = { -1.0f, 0.0f, 0.0f, 0.0f };
			static constexpr DirectX::XMVECTOR forward = { 0.0f, 0.0f, 1.0f, 0.0f };
			static constexpr DirectX::XMVECTOR back = { 0.0f, 0.0f, -1.0f, 0.0f };
		} Directions;
	};

	class TransformMatrix
	{
	public:
		TransformMatrix() = default;
		TransformMatrix(Transform& transform);
		TransformMatrix(DirectX::XMMATRIX transformMatrix, DirectX::XMMATRIX rotationMatrix);

		void Set(Transform& transform);
		void Set(DirectX::XMVECTOR position, DirectX::XMVECTOR quaterion, DirectX::XMVECTOR scale);

		inline DirectX::XMVECTOR GetTranslation() { return GetTransform().r[3]; }

		inline const DirectX::XMMATRIX& GetTransform() const { return transformMatrix; }
		inline DirectX::XMMATRIX GetTransformTransposed() const { return DirectX::XMMatrixTranspose(transformMatrix); }

		inline const DirectX::XMMATRIX& GetRotation() const { return rotationMatrix; }
		inline DirectX::XMMATRIX GetRotationTransposed() const { return DirectX::XMMatrixTranspose(rotationMatrix); }

		inline TransformMatrix GetTransposed() const { return TransformMatrix(GetTransformTransposed(), GetRotationTransposed()); }

		void OnImgui();

		inline TransformMatrix operator*(const TransformMatrix& rhs) const {
			return TransformMatrix(transformMatrix * rhs.transformMatrix, rotationMatrix * rhs.rotationMatrix);
		}

		inline void operator=(const TransformMatrix& rhs) {
			transformMatrix = rhs.transformMatrix;
			rotationMatrix  = rhs.rotationMatrix;
		}

	protected:
		// note: order is important for the constructor based on transform setting the rot matrix first then using it
		DirectX::XMMATRIX rotationMatrix;
		DirectX::XMMATRIX transformMatrix;
	};
}
