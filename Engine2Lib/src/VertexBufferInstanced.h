#pragma once

#include "pch.h"
#include "Common.h"
#include "Resources.h"
#include "DXDevice.h"
#include "Instrumentation.h"

namespace Engine2
{
	class DrawableInstanced : public Drawable
	{
	public:
		virtual void Draw() = 0;
		virtual void Draw(UINT instances) { instanceCount = instances; Draw(); };
		void SetInstanceCount(UINT instances) { instanceCount = instances; }

	protected:
		UINT instanceCount = 0;
	};

	template <typename V, D3D11_PRIMITIVE_TOPOLOGY TOP>
	class VertexBufferIndexInstanced : public DrawableInstanced
	{
	public:
		VertexBufferIndexInstanced(std::vector<V>& verticies, std::vector<unsigned int>& indicies) :
			vertexCount((UINT)verticies.size()), indxCount((UINT)indicies.size())
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

			HRESULT hr = DXDevice::GetDevice().CreateBuffer(&bufferDesc, &data, &pIndexBuffer);

			E2_ASSERT_HR(hr, "VertexBufferIndexInstanced CreateBuffer failed");

			info = "VertexBufferIndexInstanced vertex count: " + std::to_string(vertexCount) + " index count: " + std::to_string(indxCount);
		}
		template <typename D>
		ID3D11Buffer* AddInstances(std::vector<D>& bufferData, bool dynamic = false) {
			instanceCount = (UINT)bufferData.size();
			return AddBuffer<D>(bufferData, dynamic);
		}

		// use this to reference an existing buffer managed elsewhere, rather the AddInstances/AddBuffer which also creates the buffer.
		template <typename D>
		void ReferenceInstanceBuffer(ID3D11Buffer* pBuffer) {
			bufferStrides.push_back(sizeof(D));
			bufferOffsets.push_back(0);
			vertexBuffersPtrs.push_back(pBuffer);
		}

		template <typename D>
		ID3D11Buffer* AddBuffer(std::vector<D>& bufferData, bool dynamic = false) {
			D3D11_SUBRESOURCE_DATA data = {};
			data.SysMemPitch = 0;
			data.SysMemSlicePitch = 0;
			data.pSysMem = bufferData.data();

			D3D11_BUFFER_DESC bufferDesc = {};
			bufferDesc.ByteWidth = sizeof(D) * (UINT)bufferData.size();
			bufferDesc.Usage = (dynamic ? D3D11_USAGE::D3D11_USAGE_DYNAMIC : D3D11_USAGE::D3D11_USAGE_DEFAULT);
			bufferDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER;
			bufferDesc.CPUAccessFlags = (dynamic ? D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE : 0);
			bufferDesc.MiscFlags = 0;
			bufferDesc.StructureByteStride = sizeof(D);

			wrl::ComPtr<ID3D11Buffer> pBuffer;
			HRESULT hr = DXDevice::GetDevice().CreateBuffer(&bufferDesc, &data, &pBuffer);

			E2_ASSERT_HR(hr, "VertexBufferIndexInstanced AddBuffer CreateBuffer failed");

			vertexBuffers.push_back(pBuffer);
			ReferenceInstanceBuffer<D>(pBuffer.Get());

			return pBuffer.Get();
		}

		virtual void Bind() {
			DXDevice::GetContext().IASetPrimitiveTopology(topology);
			DXDevice::GetContext().IASetVertexBuffers(startSlot, (UINT)vertexBuffersPtrs.size(), vertexBuffersPtrs.data(), bufferStrides.data(), bufferOffsets.data());
			DXDevice::GetContext().IASetIndexBuffer(pIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0u);
		}

		virtual void Unbind() {
			// to do: untested
			DXDevice::GetContext().IASetVertexBuffers(startSlot, 0, nullptr, 0, 0);
			DXDevice::GetContext().IASetIndexBuffer(nullptr, DXGI_FORMAT_R32_UINT, 0u);
		}

		virtual void Draw() {
			E2_ASSERT(instanceCount > 0, "Instance count should be greater than 0, need to AddInstances");

			E2_STATS_INDEXINSTANCEDRAW(indxCount, instanceCount);
			DXDevice::GetContext().DrawIndexedInstanced(indxCount, instanceCount, 0u, 0u, 0u);
		}

		virtual void OnImgui() { ImGui::Text(info.c_str()); }

		UINT startSlot = 0u;

	protected:
		std::string info;
		std::vector<wrl::ComPtr<ID3D11Buffer>> vertexBuffers;
		std::vector<ID3D11Buffer*> vertexBuffersPtrs;
		wrl::ComPtr<ID3D11Buffer> pIndexBuffer = nullptr;
		UINT vertexCount;
		UINT indxCount;

		D3D11_PRIMITIVE_TOPOLOGY topology = TOP;

		std::vector<UINT> bufferStrides;
		std::vector<UINT> bufferOffsets;

	};

	template <typename V, typename I>
	class TriangleListIndexInstanced : public VertexBufferIndexInstanced<V, D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST>
	{
	public:
		TriangleListIndexInstanced(std::vector<V>& verticies, std::vector<unsigned int>& indicies, std::vector<I>& instances) : VertexBufferIndexInstanced<V, D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST>(verticies, indicies) {
			this->AddInstances(instances);
		}

		TriangleListIndexInstanced(std::vector<V>& verticies, std::vector<unsigned int>& indicies) : VertexBufferIndexInstanced<V, D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST>(verticies, indicies) {}
	};

}
