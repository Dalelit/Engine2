#pragma once
#include "pch.h"
#include "Common.h"

namespace Engine2
{
	class PointLight
	{
	public:
		PointLight() = default;
		PointLight(DirectX::XMVECTOR color) : color(color) {}

		void OnImgui();

		DirectX::XMVECTOR color = { 1.0f, 1.0f, 1.0f, 1.0f};
	};

	class DirectionalLight
	{
	public:
		DirectionalLight();

		void OnImgui();

		inline void SetRotation(float x, float y, float z) { SetRotation({x, y, z, 1.0f}); }
		inline void SetRotation(DirectX::XMVECTOR dir) { rotation = DirectX::XMVector3Normalize(dir); }

		inline void ShadowsOn() { castShadows = true; }
		inline void ShadowsOff() { castShadows = false; }
		inline bool IsCastingShadows() { return castShadows; }

	protected:
		DirectX::XMVECTOR color = { 1.0f, 1.0f, 1.0f, 1.0f };
		DirectX::XMVECTOR rotation;
		bool castShadows = true;

	};
}