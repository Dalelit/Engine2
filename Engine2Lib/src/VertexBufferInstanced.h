#pragma once

#include "pch.h"
#include "Common.h"
#include "Engine2.h"

namespace Engine2
{
	template <typename V, D3D11_PRIMITIVE_TOPOLOGY TOP>
	class VertexBufferIndexInstanced : public Drawable
	{
	public:
		VertexBufferIndexInstanced(std::vector<V>& verticies, std::vector<unsigned int>& indicies, UINT instanceCount) :
			vertexCount((UINT)verticies.size()), indxCount((UINT)indicies.size()), instanceCount(instanceCount)
		{
			bufferStrides[0] = sizeof(V);
			bufferOffsets[0] = 0;

			D3D11_SUBRESOURCE_DATA data = {};
			data.SysMemPitch = 0;
			data.SysMemSlicePitch = 0;
			data.pSysMem = verticies.data();

			D3D11_BUFFER_DESC bufferDesc = {};
			bufferDesc.ByteWidth = sizeof(V) * vertexCount;
			bufferDesc.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
			bufferDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER;
			bufferDesc.CPUAccessFlags = 0;
			bufferDesc.MiscFlags = 0;
			bufferDesc.StructureByteStride = sizeof(V);

			HRESULT hr = Engine::GetDevice().CreateBuffer(&bufferDesc, &data, &pVertexBuffer);

			E2_ASSERT_HR(hr, "VertexBufferIndexInstanced CreateBuffer failed");

			//D3D11_SUBRESOURCE_DATA data = {};
			//data.SysMemPitch = 0;
			//data.SysMemSlicePitch = 0;
			data.pSysMem = indicies.data();

			//D3D11_BUFFER_DESC bufferDesc = {};
			bufferDesc.ByteWidth = sizeof(unsigned int) * indxCount;
			bufferDesc.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
			bufferDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_INDEX_BUFFER;
			bufferDesc.CPUAccessFlags = 0;
			bufferDesc.MiscFlags = 0;
			bufferDesc.StructureByteStride = sizeof(unsigned int);

			hr = Engine::GetDevice().CreateBuffer(&bufferDesc, &data, &pIndexBuffer);

			E2_ASSERT_HR(hr, "VertexBufferIndexInstanced CreateBuffer failed");

			info = "VertexBufferIndexInstanced vertex count: " + std::to_string(vertexCount) + " index count: " + std::to_string(indxCount);
		}

		virtual void Bind() {
			Engine::GetContext().IASetPrimitiveTopology(topology);
			Engine::GetContext().IASetVertexBuffers(slot, numberOfBuffers, pVertexBuffer.GetAddressOf(), bufferStrides, bufferOffsets);
			Engine::GetContext().IASetIndexBuffer(pIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0u);
		}

		virtual void Unbind() {
			Engine::GetContext().IASetVertexBuffers(slot, 0, nullptr, 0, 0);
			Engine::GetContext().IASetIndexBuffer(nullptr, DXGI_FORMAT_R32_UINT, 0u);
		}

		virtual void Draw() {
			E2_STATS_INDEXINSTANCEDRAW(indxCount, instanceCount);
			Engine::GetContext().DrawIndexedInstanced(indxCount, instanceCount, 0u, 0u, 0u);
		}

		virtual void OnImgui() { ImGui::Text(info.c_str()); }

		UINT slot = 0u;
		UINT numberOfBuffers = 1u;
		UINT instanceCount = 1;

	protected:
		std::string info;
		wrl::ComPtr<ID3D11Buffer> pVertexBuffer = nullptr;
		wrl::ComPtr<ID3D11Buffer> pIndexBuffer = nullptr;
		UINT vertexCount;
		unsigned int indxCount;

		D3D11_PRIMITIVE_TOPOLOGY topology = TOP;
		UINT bufferStrides[1] = {  }; // set by vertexSize in constructor
		UINT bufferOffsets[1] = { 0 };

	};

	template <typename V>
	class TriangleListIndexInstanced : public VertexBufferIndexInstanced<V, D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST>
	{
	public:
		TriangleListIndexInstanced(std::vector<V>& verticies, std::vector<unsigned int>& indicies, UINT instanceCount = 1) : VertexBufferIndexInstanced<V, D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST>(verticies, indicies, instanceCount) {}
	};

}
