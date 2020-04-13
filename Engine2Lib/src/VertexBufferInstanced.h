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
		VertexBufferIndexInstanced(std::vector<V>& verticies, std::vector<unsigned int>& indicies) :
			vertexCount((UINT)verticies.size()), indxCount((UINT)indicies.size()), instanceCount(instanceCount)
		{
			AddBuffer<V>(verticies);

			D3D11_SUBRESOURCE_DATA data = {};
			data.SysMemPitch = 0;
			data.SysMemSlicePitch = 0;
			data.pSysMem = indicies.data();

			D3D11_BUFFER_DESC bufferDesc = {};
			bufferDesc.ByteWidth = sizeof(unsigned int) * indxCount;
			bufferDesc.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
			bufferDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_INDEX_BUFFER;
			bufferDesc.CPUAccessFlags = 0;
			bufferDesc.MiscFlags = 0;
			bufferDesc.StructureByteStride = sizeof(unsigned int);

			HRESULT hr = Engine::GetDevice().CreateBuffer(&bufferDesc, &data, &pIndexBuffer);

			E2_ASSERT_HR(hr, "VertexBufferIndexInstanced CreateBuffer failed");

			info = "VertexBufferIndexInstanced vertex count: " + std::to_string(vertexCount) + " index count: " + std::to_string(indxCount);
		}
		template <typename D>
		ID3D11Buffer* AddInstances(std::vector<D>& bufferData) {
			instanceCount = (UINT)bufferData.size();
			return AddBuffer<D>(bufferData);
		}

		template <typename D>
		ID3D11Buffer* AddBuffer(std::vector<D>& bufferData) {
			bufferStrides.push_back(sizeof(D));
			bufferOffsets.push_back(0);

			D3D11_SUBRESOURCE_DATA data = {};
			data.SysMemPitch = 0;
			data.SysMemSlicePitch = 0;
			data.pSysMem = bufferData.data();

			D3D11_BUFFER_DESC bufferDesc = {};
			bufferDesc.ByteWidth = sizeof(D) * (UINT)bufferData.size();
			bufferDesc.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
			bufferDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER;
			bufferDesc.CPUAccessFlags = 0;
			bufferDesc.MiscFlags = 0;
			bufferDesc.StructureByteStride = sizeof(D);

			wrl::ComPtr<ID3D11Buffer> pBuffer;
			HRESULT hr = Engine::GetDevice().CreateBuffer(&bufferDesc, &data, &pBuffer);

			vertexBuffers.push_back(pBuffer);
			vertexBufferPtrs.push_back(pBuffer.Get());

			E2_ASSERT_HR(hr, "VertexBufferIndexInstanced AddBuffer CreateBuffer failed");

			return pBuffer.Get();
		}

		virtual void Bind() {
			Engine::GetContext().IASetPrimitiveTopology(topology);
			Engine::GetContext().IASetVertexBuffers(startSlot, (UINT)vertexBuffers.size(), vertexBufferPtrs.data(), bufferStrides.data(), bufferOffsets.data());
			Engine::GetContext().IASetIndexBuffer(pIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0u);
		}

		virtual void Unbind() {
			throw "Not implemented";
			Engine::GetContext().IASetVertexBuffers(startSlot, 0, nullptr, 0, 0);
			Engine::GetContext().IASetIndexBuffer(nullptr, DXGI_FORMAT_R32_UINT, 0u);
		}

		virtual void Draw() {
			E2_STATS_INDEXINSTANCEDRAW(indxCount, instanceCount);
			Engine::GetContext().DrawIndexedInstanced(indxCount, instanceCount, 0u, 0u, 0u);
		}

		virtual void OnImgui() { ImGui::Text(info.c_str()); }

		void SetInstanceCount(UINT count) { instanceCount = count; }

		UINT startSlot = 0u;

	protected:
		std::string info;
		std::vector<wrl::ComPtr<ID3D11Buffer>> vertexBuffers;
		std::vector<ID3D11Buffer*> vertexBufferPtrs;
		wrl::ComPtr<ID3D11Buffer> pIndexBuffer = nullptr;
		UINT vertexCount;
		UINT indxCount;
		UINT instanceCount = 0;

		D3D11_PRIMITIVE_TOPOLOGY topology = TOP;

		std::vector<UINT> bufferStrides;
		std::vector<UINT> bufferOffsets;

	};

	template <typename V, typename I>
	class TriangleListIndexInstanced : public VertexBufferIndexInstanced<V, D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST>
	{
	public:
		TriangleListIndexInstanced(std::vector<V>& verticies, std::vector<unsigned int>& indicies, std::vector<I>& instances) : VertexBufferIndexInstanced<V, D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST>(verticies, indicies) {
			this->AddInstances<I>(instances);
		}
	};

}
