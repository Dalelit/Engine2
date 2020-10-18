#pragma once
#include "Components.h"

namespace Engine2
{
	struct RigidBody
	{
		DirectX::XMVECTOR velocity = { 0.0f, 0.0f, 0.0f, 1.0f };
		DirectX::XMVECTOR angularVelocity = { 0.0f, 0.0f, 0.0f, 0.0f };
		float mass = 1.0f;

		static DirectX::XMVECTOR gravity;

		void OnUpdate(float dt, Transform* pTransform);

		void OnImgui();
	};


}