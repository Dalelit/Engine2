#pragma once

#include "pch.h"
#include "Common.h"
#include "Engine2.h"

namespace Engine2
{
	template <typename V, D3D11_PRIMITIVE_TOPOLOGY TOP>
	class VertexBufferDynamic : public Drawable
	{
	public:
		VertexBufferDynamic(unsigned int maxVerticies) {
			
			vertexCount = 0;

			bufferStrides[0] = sizeof(V);
			bufferOffsets[0] = 0;

			D3D11_BUFFER_DESC bufferDesc = {};
			bufferDesc.ByteWidth = sizeof(V) * maxVerticies;
			bufferDesc.Usage = D3D11_USAGE::D3D11_USAGE_DYNAMIC;
			bufferDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER;
			bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE;
			bufferDesc.MiscFlags = 0;
			bufferDesc.StructureByteStride = sizeof(V);

			HRESULT hr = Engine::GetDevice().CreateBuffer(&bufferDesc, nullptr, &pVertexBuffer);

			E2_ASSERT_HR(hr, "VertexBufferDynamic CreateBuffer failed");

			info = "VertexBufferDynamic max vertex count: " + std::to_string(maxVerticies);
		}

		virtual ~VertexBufferDynamic() = default;

		virtual void Bind() {
			Engine::GetContext().IASetPrimitiveTopology(topology);
			Engine::GetContext().IASetVertexBuffers(slot, numberOfBuffers, pVertexBuffer.GetAddressOf(), bufferStrides, bufferOffsets);
		}

		virtual void Unbind() {
			Engine::GetContext().IASetVertexBuffers(slot, 0, nullptr, 0, 0);
		}

		virtual void Draw() {
			Engine::GetContext().Draw(vertexCount, 0u);
		}

		virtual void Update(V* pData, unsigned int count) {
			// to do: D3D11_MAP_WRITE_DISCARD v D3D11_MAP_WRITE_NO_OVERWRITE ?

			HRESULT hr;

			vertexCount = count;

			auto& context = Engine::GetContext();
			auto ptrBuffer = pVertexBuffer.Get();

			D3D11_MAPPED_SUBRESOURCE mappedSubResource;
			hr = context.Map(ptrBuffer, 0u, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubResource);

			E2_ASSERT_HR(hr, "VertexBufferDynamic Map failed");

			memcpy(mappedSubResource.pData, pData, sizeof(V) * count);

			context.Unmap(ptrBuffer, 0);
		}

		virtual void OnImgui() { ImGui::Text(info.c_str()); }

		UINT slot = 0u;
		UINT numberOfBuffers = 1u;

	protected:
		VertexBufferDynamic() = default;
		std::string info;
		wrl::ComPtr<ID3D11Buffer> pVertexBuffer = nullptr;

		D3D11_PRIMITIVE_TOPOLOGY topology = TOP;

		UINT bufferStrides[1] = {  }; // set by vertexSize in constructor
		UINT bufferOffsets[1] = { 0 };

		UINT vertexCount;
	};

	template <typename V>
	class LineBufferDynamic : public VertexBufferDynamic<V, D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_LINELIST>
	{
	public:
		LineBufferDynamic(unsigned int maxVertexCount) : VertexBufferDynamic<V, D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_LINELIST>(maxVertexCount) {}
	};
}