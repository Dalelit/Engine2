#pragma once
#include "pch.h"
#include "Events.h"

namespace Engine2
{
	class Camera
	{
	public:
		void OnUpdate(float dt);
		void OnInputEvent(InputEvent& event);
		void OnApplicationEvent(ApplicationEvent& event);
		void OnImugui();

		inline DirectX::XMMATRIX GetViewProjectionMatrix() { return viewProjectionMatrix; }

		inline void SetPosition(float x, float y, float z) { position = {x, y, z, 1.0f}; }

		inline void SetAspectRatio(float width, float height) { aspectRatio = width / height; }

	protected:
		float aspectRatio = 1.0f;
		float fov = DirectX::XMConvertToRadians(75.0f);
		float nearZ = 0.5f;
		float farZ = 100.0f;

		DirectX::XMVECTOR position = {};

		float yaw = 0.0f;
		float pitch = 0.0f;
		float pitchBound = DirectX::XM_PI / 2.0f - 0.001f;

		DirectX::XMMATRIX viewMatrix;
		DirectX::XMMATRIX projectionMatrix;
		DirectX::XMMATRIX viewProjectionMatrix;

		void RecalcViewMatrix();
		void RecalcProjectionMatrix();
		void RecalcViewProjectionMatrix();
	};
}
