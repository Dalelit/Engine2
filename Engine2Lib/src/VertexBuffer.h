#pragma once

#include "pch.h"
#include "Common.h"
#include "DXDevice.h"
#include "DXBuffer.h"
#include "Resources.h"
#include "Instrumentation.h"
#include "VertexBufferData.h"

namespace Engine2
{
	class VertexBuffer : public Drawable
	{
	public:
		VertexBuffer() {
			info = "Uninitialised vertex buffer";
		}

		template <typename V>
		void Initialise(D3D11_PRIMITIVE_TOPOLOGY top, std::vector<V>& verticies, bool updatable = false) {
			SetTopology(top);
			InitialiseVertexBuffer(verticies.data(), verticies.size(), updatable);
		}

		template <typename V>
		void Initialise(D3D11_PRIMITIVE_TOPOLOGY top, VertexBufferData<V>& verticies, bool updatable = false) {
			SetTopology(top);
			InitialiseVertexBuffer(verticies.VertexBegin(), verticies.VertexCount(), updatable);
		}

		template <typename V>
		void Initialise(D3D11_PRIMITIVE_TOPOLOGY top, V* verticies, size_t vCount, bool updatable = false) {
			SetTopology(top);
			InitialiseVertexBuffer(verticies, vCount, updatable);
		}

		template <typename V>
		void Update(std::vector<V>& verticies) { Update(verticies.data(), verticies.size()); }

		template <typename V>
		void Update(VertexBufferData<V>& verticies) { Update(verticies.VertexBegin(), verticies.VertexCount()); }

		template <typename V>
		void Update(V* verticies, size_t vCount) {
			// to do: D3D11_MAP_WRITE_DISCARD v D3D11_MAP_WRITE_NO_OVERWRITE ?

			if (vertexCapacity < vCount) // recreate as it's not big enough
			{
				Initialise(topology, verticies, vCount, true);
			}
			else // update it
			{
				HRESULT hr;

				vertexCount = (UINT)vCount;

				auto& context = DXDevice::GetContext();
				auto ptrBuffer = pVertexBuffer.Get();

				D3D11_MAPPED_SUBRESOURCE mappedSubResource;
				hr = context.Map(ptrBuffer, 0u, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubResource);

				E2_ASSERT_HR(hr, "VertexBufferDynamic Map failed");

				memcpy(mappedSubResource.pData, verticies, sizeof(V) * vCount);

				context.Unmap(ptrBuffer, 0);

				info = "VertexBuffer vertex count: " + std::to_string(vertexCount);
			}
		}

		virtual ~VertexBuffer() = default;

		inline void SetTopology(D3D11_PRIMITIVE_TOPOLOGY top) { topology = top; }

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

		virtual void Draw(UINT instances) {
			E2_ASSERT(instances > 0, "Instance count should be greater than 0");

			E2_STATS_INSTANCEDRAW(instances);
			DXDevice::GetContext().DrawInstanced(vertexCount, instances, 0u, 0u);
		}

		virtual void OnImgui() { ImGui::Text(info.c_str()); }

		UINT slot = 0u;
		UINT numberOfBuffers = 1u;

	protected:
		std::string info;
		wrl::ComPtr<ID3D11Buffer> pVertexBuffer = nullptr;

		D3D11_PRIMITIVE_TOPOLOGY topology;

		UINT bufferStrides[1] = {  }; // set by vertexSize in constructor
		UINT bufferOffsets[1] = { 0 };

		UINT vertexCount = 0;
		UINT vertexCapacity = 0;

		template <typename V>
		void InitialiseVertexBuffer(V* verticies, size_t vCount, bool updatable) {
			if (pVertexBuffer) pVertexBuffer.Reset();

			bufferStrides[0] = sizeof(V);
			bufferOffsets[0] = 0;

			vertexCount = (UINT)vCount;
			vertexCapacity = vertexCount;

			D3D11_SUBRESOURCE_DATA data = {};
			data.SysMemPitch = 0;
			data.SysMemSlicePitch = 0;
			data.pSysMem = verticies;

			D3D11_BUFFER_DESC bufferDesc = {};
			bufferDesc.ByteWidth = sizeof(V) * vertexCount;
			bufferDesc.Usage = (updatable ? D3D11_USAGE::D3D11_USAGE_DYNAMIC : D3D11_USAGE::D3D11_USAGE_DEFAULT);
			bufferDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER;
			bufferDesc.CPUAccessFlags = (updatable ? D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE : 0);
			bufferDesc.MiscFlags = 0;
			bufferDesc.StructureByteStride = sizeof(V);

			HRESULT hr = DXDevice::GetDevice().CreateBuffer(&bufferDesc, &data, &pVertexBuffer);

			E2_ASSERT_HR(hr, "VertexBuffer CreateBuffer failed");

			info = "VertexBuffer vertex count: " + std::to_string(vertexCount);
		}
	};

	class VertexBufferIndex : public VertexBuffer
	{
	public:
		VertexBufferIndex() {
			this->info = "Uninitialised vertex buffer index";
		}

		template <typename V>
		void Initialise(D3D11_PRIMITIVE_TOPOLOGY top, std::vector<V>& verticies, std::vector<unsigned int>& indicies, bool updatable = false) {
			SetTopology(top);
			InitialiseVertexBuffer(verticies.data(), verticies.size(), updatable);
			InitialiseIndexBuffer(indicies.data(), indicies.size(), updatable);
		}

		template <typename V>
		void Initialise(D3D11_PRIMITIVE_TOPOLOGY top, VertexIndexBufferData<V>& data, bool updatable = false) {
			SetTopology(top);
			InitialiseVertexBuffer(data.VertexBegin(), data.VertexCount(), updatable);
			InitialiseIndexBuffer(data.IndexBegin(), data.IndexCount(), updatable);
		}

		template <typename V>
		void Initialise(D3D11_PRIMITIVE_TOPOLOGY top, V* verticies, size_t vCount, uint32_t* indicies, size_t iCount, bool updatable = false) {
			SetTopology(top);
			InitialiseVertexBuffer(verticies, vCount, updatable);
			InitialiseIndexBuffer(indicies, iCount, updatable);
		}

		template <typename V>
		void Update(std::vector<V>& verticies, std::vector<unsigned int>& indicies) { Update(verticies.data(), verticies.size(), indicies.data(), indicies.size()); }

		template <typename V>
		void Update(VertexIndexBufferData<V>& data) { Update(data.VertexBegin(), data.VertexCount(), data.IndexBegin(), data.IndexCount()); }

		template <typename V>
		void Update(V* verticies, size_t vertexCount, uint32_t* indicies, size_t iCount) {

			VertexBuffer::Update<V>(verticies, vertexCount);

			if (indxCapacity < iCount) // recreate as it's not big enough
			{
				InitialiseIndexBuffer(indicies, iCount, true);
			}
			else // update
			{
				// to do: D3D11_MAP_WRITE_DISCARD v D3D11_MAP_WRITE_NO_OVERWRITE ?

				HRESULT hr;

				indxCount = (UINT)iCount;

				auto& context = DXDevice::GetContext();
				auto ptrBuffer = pIndexBuffer.Get();

				D3D11_MAPPED_SUBRESOURCE mappedSubResource;
				hr = context.Map(ptrBuffer, 0u, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubResource);

				E2_ASSERT_HR(hr, "VertexBufferIndex Map failed");

				memcpy(mappedSubResource.pData, indicies, sizeof(uint32_t) * indxCount);

				context.Unmap(ptrBuffer, 0);

				this->info += " index count: " + std::to_string(indxCount);
			}
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

		void Draw(UINT instances) {
			E2_ASSERT(instances > 0, "Instance count should be greater than 0");

			E2_STATS_INDEXINSTANCEDRAW(indxCount, instances);
			DXDevice::GetContext().DrawIndexedInstanced(indxCount, instances, 0u, 0u, 0u);
		}


	protected:
		wrl::ComPtr<ID3D11Buffer> pIndexBuffer = nullptr;
		UINT indxCount = 0;
		UINT indxCapacity = 0;

		void InitialiseIndexBuffer(uint32_t* indicies, size_t iCount, bool updatable) {

			if (pIndexBuffer) pIndexBuffer.Reset();

			HRESULT hr;

			indxCount = (UINT)iCount;
			indxCapacity = indxCount;

			D3D11_SUBRESOURCE_DATA data = {};
			data.SysMemPitch = 0;
			data.SysMemSlicePitch = 0;
			data.pSysMem = indicies;

			D3D11_BUFFER_DESC bufferDesc = {};
			bufferDesc.ByteWidth = sizeof(unsigned int) * indxCount;
			bufferDesc.Usage = (updatable ? D3D11_USAGE::D3D11_USAGE_DYNAMIC : D3D11_USAGE::D3D11_USAGE_DEFAULT);
			bufferDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_INDEX_BUFFER;
			bufferDesc.CPUAccessFlags = (updatable ? D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE : 0);
			bufferDesc.MiscFlags = 0;
			bufferDesc.StructureByteStride = sizeof(unsigned int);

			hr = DXDevice::GetDevice().CreateBuffer(&bufferDesc, &data, &pIndexBuffer);

			E2_ASSERT_HR(hr, "VertexBufferIndex CreateBuffer failed");

			this->info += " index count: " + std::to_string(indxCount);
		}
	};
}