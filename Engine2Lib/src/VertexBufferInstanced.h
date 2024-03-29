#pragma once

#include "pch.h"
#include "Common.h"
#include "DXDevice.h"
#include "DXBuffer.h"
#include "Instrumentation.h"

namespace Engine2
{
	class VertexBufferIndexInstanced
	{
	public:
		VertexBufferIndexInstanced()
		{
			info = "Uninitialised vertex buffer instanced";
		}

		template <typename V>
		void Initialise(D3D11_PRIMITIVE_TOPOLOGY topology, std::vector<V>& verticies, std::vector<unsigned int>& indicies) {
			this->topology = topology;
			vertexBuffer.InitBuffer<V>(verticies, false, D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER);
			vertexBuffersPtrs[0] = vertexBuffer.GetPtr();
			bufferStrides[0] = sizeof(V);
			bufferOffsets[0] = 0;

			indexBuffer.InitBuffer<UINT>(indicies, false, D3D11_BIND_FLAG::D3D11_BIND_INDEX_BUFFER);

			vertexCount = (UINT)verticies.size();
			indxCount = (UINT)indicies.size();
			info = "VertexBufferIndexInstanced vertex count: " + std::to_string(vertexCount) + " index count: " + std::to_string(indxCount);
		}

		template <typename I>
		void SetInstances(std::vector<I>& bufferData, bool dynamic = true, bool unorderedAcces = false) {
			instanceCount = (UINT)bufferData.size();

			UINT bindFlags = D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER;
			if (unorderedAcces) bindFlags |= D3D11_BIND_FLAG::D3D11_BIND_UNORDERED_ACCESS;
			
			instanceBuffer.InitBuffer<I>(bufferData, dynamic, bindFlags);
			vertexBuffersPtrs[1] = instanceBuffer.GetPtr();
			bufferStrides[1] = sizeof(I);
			bufferOffsets[1] = 0;
		}

		template <typename I>
		void SetInstances(size_t maxInstances) {
			instanceCount = (UINT)maxInstances;

			instanceBuffer.InitBuffer<I>(maxInstances, true, D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER);
			vertexBuffersPtrs[1] = instanceBuffer.GetPtr();
			bufferStrides[1] = sizeof(I);
			bufferOffsets[1] = 0;
		}

		template <typename I>
		void UpdateInstanceBuffer(std::vector<I>& source, size_t size) {
			instanceBuffer.UpdateBuffer(source, size);
		}

		DXBuffer& GetInstanceBuffer() { return instanceBuffer; }

		virtual void Bind() {
			DXDevice::GetContext().IASetPrimitiveTopology(topology);
			DXDevice::GetContext().IASetVertexBuffers(startSlot, 2, vertexBuffersPtrs, bufferStrides, bufferOffsets);
			DXDevice::GetContext().IASetIndexBuffer(indexBuffer.GetPtr(), DXGI_FORMAT_R32_UINT, 0u);
		}

		virtual void Unbind() {
			// to do: untested
			DXDevice::GetContext().IASetVertexBuffers(startSlot, 0, nullptr, 0, 0);
			DXDevice::GetContext().IASetIndexBuffer(nullptr, DXGI_FORMAT_R32_UINT, 0u);
		}

		void SetDrawCount(UINT instances) { instanceCount = instances; }

		void Draw() { Draw(instanceCount); };

		void Draw(UINT instances) {
			E2_ASSERT(instances > 0, "Instance count should be greater than 0, need to AddInstances");

			E2_STATS_INDEXINSTANCEDRAW(indxCount, instances);
			DXDevice::GetContext().DrawIndexedInstanced(indxCount, instances, 0u, 0u, 0u);
		}

		virtual void OnImgui() { ImGui::Text(info.c_str()); }

		UINT startSlot = 0u;

	protected:
		UINT instanceCount = 0;
		std::string info;

		DXBuffer vertexBuffer;
		DXBuffer indexBuffer;
		DXBuffer instanceBuffer;

		ID3D11Buffer* vertexBuffersPtrs[2];
		UINT bufferStrides[2];
		UINT bufferOffsets[2];

		UINT vertexCount;
		UINT indxCount;

		D3D11_PRIMITIVE_TOPOLOGY topology;
	};

}
