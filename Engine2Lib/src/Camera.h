#pragma once
#include "pch.h"
#include "Events.h"

namespace Engine2
{
	class Camera
	{
	public:
		Camera(std::string name) : name(name) {}
		virtual ~Camera() = default;

		void LoadViewProjectionMatrixT(DirectX::XMMATRIX& vpMatrix);

		void OnImugui();

		inline void Translate(float x, float y, float z)   { position.m128_f32[0] += x; position.m128_f32[1] += y; position.m128_f32[2] += z; }
		inline void SetPosition(float x, float y, float z) { position = {x, y, z, 1.0f}; }
		void Move(float forwardDist, float rightDist, float upDist);

		void LookAt(float x, float y, float z);

		inline void Rotate(float yawRads, float pitchRads)      { yaw += yawRads; pitch += pitchRads; WrapYaw(); ClampPitch(); }
		inline void SetRotation(float yawRads, float pitchRads) { yaw = yawRads; pitch = pitchRads; WrapYaw(); ClampPitch(); }

		inline void SetAspectRatio(float ratio) { aspectRatio = ratio; }
		inline bool IsAspectRatioLockedToScreen() { return aspectRatioLockedToScreen; }
		inline bool SetAspectRatioLockedToScreen(bool locked = true) { aspectRatioLockedToScreen = locked; }

		inline DirectX::XMVECTOR GetPosition() { return position; }
		inline std::string& GetName() { return name; }

	protected:
		std::string name;
		float aspectRatio = 1.0f;
		float fov = DirectX::XMConvertToRadians(75.0f);
		float nearZ = 0.5f;
		float farZ = 10000.0f;
		bool aspectRatioLockedToScreen = true;

		DirectX::XMVECTOR position = {};
		DirectX::XMVECTOR direction = {};

		float yaw = 0.0f;
		float pitch = 0.0f;
		float pitchBound = DirectX::XM_PIDIV2 - 0.001f;

		inline void ClampPitch() { pitch = std::clamp(pitch, -pitchBound, pitchBound); }
		inline void WrapYaw() { yaw = fmodf((float)yaw, DirectX::XM_2PI); if (yaw < 0.0f) yaw += DirectX::XM_2PI; }
	};
}
