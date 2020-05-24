#pragma once
#include "pch.h"

namespace Engine2
{
	namespace Math
	{
		inline DirectX::XMMATRIX TranslationMatrix(DirectX::XMFLOAT3 position) { return DirectX::XMMatrixTranslation(position.x, position.y, position.z); }
		inline DirectX::XMMATRIX TranslationMatrix(DirectX::XMVECTOR position) { return DirectX::XMMatrixTranslationFromVector(position); }
	}
}