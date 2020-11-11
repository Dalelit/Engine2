#pragma once
#include <vector>
#include <d3d11_3.h>

namespace Engine2
{
	namespace VertexLayout
	{
		struct PositionNormalColor
		{
			DirectX::XMFLOAT3 position;
			DirectX::XMFLOAT3 normal;
			DirectX::XMFLOAT4 color;

			static std::vector<D3D11_INPUT_ELEMENT_DESC> Layout;
		};

		struct PositionNormalTexture
		{
			DirectX::XMFLOAT3 position;
			DirectX::XMFLOAT3 normal;
			DirectX::XMFLOAT2 texcoord;

			static std::vector<D3D11_INPUT_ELEMENT_DESC> Layout;
		};

	}
}