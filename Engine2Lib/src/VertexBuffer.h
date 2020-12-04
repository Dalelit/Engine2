#pragma once

#include "pch.h"
#include "Common.h"
#include "DXDevice.h"
#include "DXBuffer.h"
#include "Resources.h"
#include "Instrumentation.h"

namespace Engine2
{
	class VertexBuffer : public Drawable
	{
	public:
		VertexBuffer() {
			info = "Uninitialised vertex buffer";
		}

		template <typename V>
		void Initialise(D3D11_PRIMITIVE_TOPOLOGY top, std::vector<V>& verticies, bool updatableVerticies = false) {

			topology = top;

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

		template <typename V>
		void Update(std::vector<V>& verticies) {
			// to do: D3D11_MAP_WRITE_DISCARD v D3D11_MAP_WRITE_NO_OVERWRITE ?

			HRESULT hr;

			vertexCount = verticies.size();

			auto& context = DXDevice::GetContext();
			auto ptrBuffer = pVertexBuffer.Get();

			D3D11_MAPPED_SUBRESOURCE mappedSubResource;
			hr = context.Map(ptrBuffer, 0u, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubResource);

			E2_ASSERT_HR(hr, "VertexBufferDynamic Map failed");

			memcpy(mappedSubResource.pData, verticies.data(), sizeof(V) * verticies.size());

			context.Unmap(ptrBuffer, 0);
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
		std::string info;
		wrl::ComPtr<ID3D11Buffer> pVertexBuffer = nullptr;

		D3D11_PRIMITIVE_TOPOLOGY topology;

		UINT bufferStrides[1] = {  }; // set by vertexSize in constructor
		UINT bufferOffsets[1] = { 0 };

		UINT vertexCount;
	};

	class VertexBufferIndex : public VertexBuffer
	{
	public:
		VertexBufferIndex() {
			this->info = "Uninitialised vertex buffer index";
		}

		template <typename V>
		void Initialise(D3D11_PRIMITIVE_TOPOLOGY top, std::vector<V>& verticies, std::vector<unsigned int>& indicies, bool updatableVerticies = false, bool updatableIndicies = false) {

			topology = top;

			VertexBuffer::Initialise<V>(top, verticies, updatableVerticies); // call base initialise

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

		template <typename V>
		void Update(std::vector<V>& verticies, std::vector<unsigned int>& indicies) {
			// to do: D3D11_MAP_WRITE_DISCARD v D3D11_MAP_WRITE_NO_OVERWRITE ?

			VertexBuffer::Update<V>(verticies);
			
			HRESULT hr;

			indxCount = (unsigned int)indicies.size();

			auto& context = DXDevice::GetContext();
			auto ptrBuffer = pIndexBuffer.Get();

			D3D11_MAPPED_SUBRESOURCE mappedSubResource;
			hr = context.Map(ptrBuffer, 0u, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubResource);

			E2_ASSERT_HR(hr, "VertexBufferIndex Map failed");

			memcpy(mappedSubResource.pData, indicies.data(), sizeof(unsigned int) * indicies.size());

			context.Unmap(ptrBuffer, 0);
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
}