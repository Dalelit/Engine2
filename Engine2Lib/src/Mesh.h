#pragma once

#include "pch.h"
#include "Common.h"
#include "Engine2.h"

namespace Engine2
{
	class Mesh : public Drawable
	{
	public:
		virtual ~Mesh() = default;
		virtual void OnImgui() { ImGui::Text(info.c_str()); }

	protected:
		std::string info;
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

			E2_ASSERT_HR(hr, "Mesh vertex CreateBuffer failed");

			info = "Mesh TriangleList vertex count: " + std::to_string(vertexCount);
		}

		void Bind() {
			Engine::GetContext().IASetPrimitiveTopology(topology);
			Engine::GetContext().IASetVertexBuffers(slot, numberOfBuffers, pVertexBuffer.GetAddressOf(), bufferStrides, bufferOffsets);
		}

		void Unbind() {
			// to do: untested
			Engine::GetContext().IASetVertexBuffers(slot, numberOfBuffers, nullptr, 0, 0);
		}

		void Draw() {
			Engine::GetContext().Draw(vertexCount, 0u);
		}

		UINT slot = 0u;
		UINT numberOfBuffers = 1u;

	protected:
		MeshTriangleList() = default;
		wrl::ComPtr<ID3D11Buffer> pVertexBuffer = nullptr;

		D3D11_PRIMITIVE_TOPOLOGY topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

		UINT bufferStrides[1] = {  }; // set by vertexSize in constructor
		UINT bufferOffsets[1] = { 0 };

		UINT vertexCount;
	};

	template <typename V>
	class MeshTriangleIndexList : public MeshTriangleList<V>
	{
	public:
		MeshTriangleIndexList(std::vector<V>& verticies, std::vector<unsigned int>& indicies) :
			MeshTriangleList<V>(verticies)
		{
			HRESULT hr;

			indxCount = (unsigned int)indicies.size();

			D3D11_SUBRESOURCE_DATA data = {};
			data.SysMemPitch = 0;
			data.SysMemSlicePitch = 0;
			data.pSysMem = indicies.data();

			D3D11_BUFFER_DESC bufferDesc = {};
			bufferDesc.ByteWidth = sizeof(unsigned int) * indxCount;
			bufferDesc.Usage = D3D11_USAGE_DEFAULT;
			bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
			bufferDesc.CPUAccessFlags = 0;
			bufferDesc.MiscFlags = 0;
			bufferDesc.StructureByteStride = sizeof(unsigned int);

			hr = Engine::GetDevice().CreateBuffer(&bufferDesc, &data, &pIndexBuffer);

			E2_ASSERT_HR(hr, "Mesh index CreateBuffer failed");

			this->info += " index count: " + std::to_string(indxCount);
		}

		void Bind() {
			Engine::GetContext().IASetPrimitiveTopology(this->topology);
			Engine::GetContext().IASetVertexBuffers(this->slot, this->numberOfBuffers, this->pVertexBuffer.GetAddressOf(), this->bufferStrides, this->bufferOffsets);
			Engine::GetContext().IASetIndexBuffer(pIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0u);
		}

		void Unbind() {
			// to do: untested
			Engine::GetContext().IASetVertexBuffers(this->slot, 0, nullptr, 0, 0);
			Engine::GetContext().IASetIndexBuffer(nullptr, DXGI_FORMAT_R32_UINT, 0u);
		}

		void Draw() {
			Engine::GetContext().DrawIndexed(indxCount, 0u, 0u);
		}

	protected:
		wrl::ComPtr<ID3D11Buffer> pIndexBuffer = nullptr;
		unsigned int indxCount = 0;
	};

}