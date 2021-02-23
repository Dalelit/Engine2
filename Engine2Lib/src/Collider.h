#pragma once
#include "UtilMath.h"
#include "Serialiser.h"

namespace Engine2
{
	// To Do: Think through a better way. Separate components per type of collider vs this overloaded approach/hack?

	class Collider
	{
	public:
		enum class ColliderType { sphere, capsule, box, plane };

		Collider();
		void OnImgui();
		void Serialise(Serialisation::INode& node);

		inline ColliderType GetType() const { return type; }

		void InitialiseAsSphere();
		void InitialiseAsBox();
		void InitialiseAsCapsule();
		void InitialiseAsPlane();

		inline DirectX::XMFLOAT3 Centre() const { return centre; }

		// sphere, capsule
		inline float Radius() const { return extents.x; }
		inline float Radius(DirectX::XMVECTOR scale) const { return extents.x * Math::GetMaxFloatVector3(scale); }
		inline void SetRadius(float radius) { extents.x = radius; }

		// capsule
		inline float HalfHeight() const { return extents.y; }
		inline void SetHalfHeight(float halfHeight) { extents.y = halfHeight; }

		// box
		inline DirectX::XMFLOAT3 HalfExtents() const { return extents; }
		inline DirectX::XMFLOAT3 HalfExtents(DirectX::XMVECTOR scale) const { return { extents.x * scale.m128_f32[0], extents.x * scale.m128_f32[1], extents.x * scale.m128_f32[2]}; } // to do: performance... load, simd, store better than this?
		inline void SetHalfExtents(float halfExtents) { extents.x = halfExtents; extents.y = halfExtents; extents.z = halfExtents; }

	protected:
		ColliderType type;
		bool showCollider = false;

		DirectX::XMFLOAT3 centre = { 0.0f, 0.0f, 0.0f };

		DirectX::XMFLOAT3 extents = { 1.0f, 1.0f, 1.0f };
		// sphere - radius
		// capsule - radius, half-height
		// box - half extents x,y,z, axis
		// plane - axis
		// ignore axis for now
	};
}