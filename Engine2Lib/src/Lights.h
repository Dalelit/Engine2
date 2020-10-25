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

		inline DirectX::XMVECTOR GetColor() { return color; }

		void OnImgui();

	protected:
		DirectX::XMVECTOR color = { 1.0f, 1.0f, 1.0f, 1.0f};
	};
}