#pragma once
#include "pch.h"

namespace Engine2
{
	namespace VertexLayout
	{
		using namespace DirectX;

		class PositionNormalColor
		{
		public:
			struct Vertex {
				XMFLOAT3 position;
				XMFLOAT3 normal;
				XMFLOAT4 color;
			};

			static std::vector<D3D11_INPUT_ELEMENT_DESC> GetLayout() {
				return {
					{"Position", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
					{"Normal",   0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
					{"Color",    0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
				};
			}
		};
	};
}