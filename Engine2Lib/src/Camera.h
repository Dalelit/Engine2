#pragma once
#include "pch.h"
#include "Events.h"

namespace Engine2
{
	class Camera
	{
	public:
		void LoadViewProjectionMatrixT(DirectX::XMMATRIX& vpMatrix);

		void ImuguiWindow(bool* pOpen);

		inline void Translate(float x, float y, float z)   { position.m128_f32[0] += x; position.m128_f32[1] += y; position.m128_f32[2] += z; }
		inline void SetPosition(float x, float y, float z) { position = {x, y, z, 1.0f}; }

		inline void TranslateForward(float dist) { position = DirectX::XMVectorAdd(position, DirectX::XMVectorScale(ForwardDirection(), dist)); } // position += dist * ForwardDirection();
		inline void TranslateRight(float dist)   { position = DirectX::XMVectorAdd(position, DirectX::XMVectorScale(RightDirection(), dist)); }   // position += dist * RightDirection();

		inline void Rotate(float yawRads, float pitchRads)      { yaw += yawRads; pitch += pitchRads; WrapYaw(); ClampPitch(); }
		inline void SetRotation(float yawRads, float pitchRads) { yaw = yawRads; pitch = pitchRads; WrapYaw(); ClampPitch(); }

		inline void SetAspectRatio(float ratio) { aspectRatio = ratio; }

		inline DirectX::XMVECTOR ForwardDirection() { return forward; }
		inline DirectX::XMVECTOR RightDirection()   { return DirectX::XMVector3Normalize(DirectX::XMVector3Cross({ 0.0f, 1.0f, 0.0f, 1.0f }, forward)); }

	protected:
		float aspectRatio = 1.0f;
		float fov = DirectX::XMConvertToRadians(75.0f);
		float nearZ = 0.5f;
		float farZ = 100.0f;

		DirectX::XMVECTOR position = {};
		DirectX::XMVECTOR forward = {};

		float yaw = 0.0f;
		float pitch = 0.0f;
		float pitchBound = DirectX::XM_PI / 2.0f - 0.001f;

		inline void ClampPitch() { pitch = std::clamp(pitch, -pitchBound, pitchBound); }
		inline void WrapYaw() { yaw = fmodf((float)yaw, DirectX::XM_2PI); if (yaw < 0.0f) yaw += DirectX::XM_2PI; }
	};
}
