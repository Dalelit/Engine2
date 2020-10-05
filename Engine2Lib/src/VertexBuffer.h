#pragma once

#include "pch.h"
#include "Common.h"
#include "DXDevice.h"
#include "DXBuffer.h"
#include "Resources.h"
#include "Instrumentation.h"

namespace Engine2
{
	template <typename V, D3D11_PRIMITIVE_TOPOLOGY TOP>
	class VertexBuffer : public Drawable
	{
	public:
		VertexBuffer(std::vector<V>& verticies, bool updatableVerticies = false) {

			bufferStrides[0] = sizeof(V);
			bufferOffsets[0] = 0;

			vertexCount = (UINT)verticies.size();

			D3D11_SUBRESOURCE_DATA data = {};
			data.SysMemPitch = 0;
			data.SysMemSlicePitch = 0;
			data.pSysMem = verticies.data();

			D3D11_BUFFER_DESC bufferDesc = {};
			bufferDesc.ByteWidth = sizeof(V) * vertexCount;
			bufferDesc.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
			bufferDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER;
			bufferDesc.CPUAccessFlags = (updatableVerticies ? D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE : 0);
			bufferDesc.MiscFlags = 0;
			bufferDesc.StructureByteStride = sizeof(V);

			HRESULT hr = DXDevice::GetDevice().CreateBuffer(&bufferDesc, &data, &pVertexBuffer);

			E2_ASSERT_HR(hr, "VertexBuffer CreateBuffer failed");

			info = "VertexBuffer vertex count: " + std::to_string(vertexCount);
		}

		virtual ~VertexBuffer() = default;

		virtual void Bind() {
			DXDevice::GetContext().IASetPrimitiveTopology(topology);
			DXDevice::GetContext().IASetVertexBuffers(slot, numberOfBuffers, pVertexBuffer.GetAddressOf(), bufferStrides, bufferOffsets);
		}

		virtual void Unbind() {
			DXDevice::GetContext().IASetVertexBuffers(slot, 0, nullptr, 0, 0);
		}

		virtual void Draw() {
			E2_STATS_VERTEXDRAW(vertexCount);
			DXDevice::GetContext().Draw(vertexCount, 0u);
		}

		virtual void OnImgui() { ImGui::Text(info.c_str()); }

		UINT slot = 0u;
		UINT numberOfBuffers = 1u;

	protected:
		VertexBuffer() = default;
		std::string info;
		wrl::ComPtr<ID3D11Buffer> pVertexBuffer = nullptr;

		D3D11_PRIMITIVE_TOPOLOGY topology = TOP;

		UINT bufferStrides[1] = {  }; // set by vertexSize in constructor
		UINT bufferOffsets[1] = { 0 };

		UINT vertexCount;
	};

	template <typename V, D3D11_PRIMITIVE_TOPOLOGY TOP>
	class VertexBufferIndex : public VertexBuffer<V, TOP>
	{
	public:
		VertexBufferIndex(std::vector<V>& verticies, std::vector<unsigned int>& indicies, bool updatableVerticies = false, bool updatableIndicies = false) :
			VertexBuffer<V, TOP>(verticies, updatableVerticies)
		{
			HRESULT hr;

			indxCount = (unsigned int)indicies.size();

			D3D11_SUBRESOURCE_DATA data = {};
			data.SysMemPitch = 0;
			data.SysMemSlicePitch = 0;
			data.pSysMem = indicies.data();

			D3D11_BUFFER_DESC bufferDesc = {};
			bufferDesc.ByteWidth = sizeof(unsigned int) * indxCount;
			bufferDesc.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
			bufferDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_INDEX_BUFFER;
			bufferDesc.CPUAccessFlags = (updatableIndicies ? D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE : 0);
			bufferDesc.MiscFlags = 0;
			bufferDesc.StructureByteStride = sizeof(unsigned int);

			hr = DXDevice::GetDevice().CreateBuffer(&bufferDesc, &data, &pIndexBuffer);

			E2_ASSERT_HR(hr, "VertexBufferIndex CreateBuffer failed");

			this->info += " index count: " + std::to_string(indxCount);
		}

		void Bind() {
			DXDevice::GetContext().IASetPrimitiveTopology(this->topology);
			DXDevice::GetContext().IASetVertexBuffers(this->slot, this->numberOfBuffers, this->pVertexBuffer.GetAddressOf(), this->bufferStrides, this->bufferOffsets);
			DXDevice::GetContext().IASetIndexBuffer(pIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0u);
		}

		void Unbind() {
			// to do: untested
			DXDevice::GetContext().IASetVertexBuffers(this->slot, 0, nullptr, 0, 0);
			DXDevice::GetContext().IASetIndexBuffer(nullptr, DXGI_FORMAT_R32_UINT, 0u);
		}

		void Draw() {
			E2_STATS_INDEXDRAW(indxCount);
			DXDevice::GetContext().DrawIndexed(indxCount, 0u, 0u);
		}

	protected:
		wrl::ComPtr<ID3D11Buffer> pIndexBuffer = nullptr;
		unsigned int indxCount = 0;
	};

	template <typename V>
	class MeshTriangleList : public VertexBuffer<V, D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST>
	{
	public:
		MeshTriangleList(std::vector<V>& verticies) : VertexBuffer<V, D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST>(verticies) {}
	};

	template <typename V>
	class MeshTriangleIndexList : public VertexBufferIndex<V, D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST>
	{
	public:
		MeshTriangleIndexList(std::vector<V>& verticies, std::vector<unsigned int>& indicies) : VertexBufferIndex<V, D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST>(verticies, indicies) {}
	};

	template <typename V>
	class WireframeList : public VertexBuffer<V, D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_LINELIST>
	{
	public:
		WireframeList(std::vector<V>& verticies) : VertexBuffer<V, D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_LINELIST>(verticies) {}
	};

	template <typename V>
	class WireframeIndexList : public VertexBufferIndex<V, D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_LINELIST>
	{
	public:
		WireframeIndexList(std::vector<V>& verticies, std::vector<unsigned int>& indicies) : VertexBufferIndex<V, D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_LINELIST>(verticies, indicies) {}
	};

}