#pragma once
#include "pch.h"
#include "Common.h"

namespace Engine2
{
	class PointLight
	{
	public:
		PointLight(DirectX::XMVECTOR position, DirectX::XMVECTOR color) : position(position), color(color) {}

		inline DirectX::XMVECTOR GetPosition() { return position; }
		inline DirectX::XMVECTOR GetColor() { return color; }

		inline DirectX::XMMATRIX GetTransform() { return DirectX::XMMatrixTranslationFromVector(position); }

		void OnImgui();

	protected:
		bool active = true;
		std::string id = "PointLight";

		DirectX::XMVECTOR position;
		DirectX::XMVECTOR color;

	};
}