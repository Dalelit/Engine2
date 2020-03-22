#pragma once

#include "pch.h"
#include "Common.h"
#include "Engine2.h"

namespace Engine2
{
	class Mesh
	{
	public:
		virtual ~Mesh() = default;
		virtual void Bind() = 0;
		virtual void Draw() = 0;
	};

	template <typename V>
	class MeshTriangleList : public Mesh
	{
	public:
		MeshTriangleList(std::vector<V>& verticies) {

			bufferStrides[0] = sizeof(V);
			bufferOffsets[0] = 0;

			vertexCount = (UINT)verticies.size();

			// create triangle data
			D3D11_SUBRESOURCE_DATA data = {};
			data.SysMemPitch = 0;
			data.SysMemSlicePitch = 0;
			data.pSysMem = verticies.data();

			// create the triangle buffer
			D3D11_BUFFER_DESC bufferDesc = {};
			bufferDesc.ByteWidth = sizeof(V) * vertexCount;
			bufferDesc.Usage = D3D11_USAGE_DEFAULT;
			bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			bufferDesc.CPUAccessFlags = 0;
			bufferDesc.MiscFlags = 0;
			bufferDesc.StructureByteStride = sizeof(V);

			HRESULT hr = Engine::GetDevice().CreateBuffer(&bufferDesc, &data, &pVertexBuffer);

			E2_ASSERT_HR(hr, "Mesh CreateBuffer failed");
		}

		void Bind() {
			Engine::GetContext().IASetPrimitiveTopology(topology);
			Engine::GetContext().IASetVertexBuffers(slot, numberOfBuffers, pVertexBuffer.GetAddressOf(), bufferStrides, bufferOffsets);
		}

		void Draw() {
			Engine::GetContext().Draw(vertexCount, 0u);
		}

		std::vector<V> verticies;

		UINT slot = 0u;
		UINT numberOfBuffers = 1u;

	protected:
		wrl::ComPtr<ID3D11Buffer> pVertexBuffer = nullptr;

		D3D11_PRIMITIVE_TOPOLOGY topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

		UINT bufferStrides[1] = {  }; // set by vertexSize in constructor
		UINT bufferOffsets[1] = { 0 };

		UINT vertexCount;
	};
}