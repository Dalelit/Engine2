#pragma once
#include "pch.h"
#include "DXDevice.h"
#include "Common.h"

namespace Engine2
{

	// DirectX buffer for
	class DXBuffer
	{
	public:
		// uninitialised buffer
		DXBuffer() = default;
		//DXBuffer(DXBuffer&) = delete;
		//DXBuffer(DXBuffer&&) = delete;
		virtual ~DXBuffer() = default;

		// initialised empty buffer
		template <typename T>
		void InitBuffer(size_t size, bool dynamic, D3D11_BIND_FLAG bindFlags) {
			D3D11_BUFFER_DESC bufferDesc = {};
			bufferDesc.ByteWidth = sizeof(T) * (UINT)size;
			bufferDesc.Usage = (dynamic ? D3D11_USAGE::D3D11_USAGE_DYNAMIC : D3D11_USAGE::D3D11_USAGE_DEFAULT);
			bufferDesc.BindFlags = bindFlags;
			bufferDesc.CPUAccessFlags = (dynamic ? D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE : 0);
			bufferDesc.MiscFlags = 0;
			bufferDesc.StructureByteStride = sizeof(T);

			HRESULT hr = DXDevice::GetDevice().CreateBuffer(&bufferDesc, nullptr, &comptrBuffer);

			E2_ASSERT_HR(hr, "DXBuffer CreateEmptyBuffer failed");

			capacity = size;
			elementSize = sizeof(T);
			isDynamic = dynamic;
		}

		// initialised buffer with multiple element content
		template <typename T>
		void InitBuffer(std::vector<T>& source, bool dynamic, D3D11_BIND_FLAG bindFlags) {
			D3D11_SUBRESOURCE_DATA data = {};
			data.SysMemPitch = 0;
			data.SysMemSlicePitch = 0;
			data.pSysMem = source.data();

			D3D11_BUFFER_DESC bufferDesc = {};
			bufferDesc.ByteWidth = sizeof(T) * (UINT)source.size();
			bufferDesc.Usage = (dynamic ? D3D11_USAGE::D3D11_USAGE_DYNAMIC : D3D11_USAGE::D3D11_USAGE_DEFAULT);
			bufferDesc.BindFlags = bindFlags;
			bufferDesc.CPUAccessFlags = (dynamic ? D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE : 0);
			bufferDesc.MiscFlags = 0;
			bufferDesc.StructureByteStride = sizeof(T);

			HRESULT hr = DXDevice::GetDevice().CreateBuffer(&bufferDesc, &data, &comptrBuffer);

			E2_ASSERT_HR(hr, "DXBuffer CreateBuffer failed");

			capacity = source.size();
			elementSize = sizeof(T);
			isDynamic = dynamic;
		}

		// initialised buffer with single element content
		template <typename T>
		void InitBuffer(T& source, bool dynamic, D3D11_BIND_FLAG bindFlags) {
			D3D11_SUBRESOURCE_DATA data = {};
			data.SysMemPitch = 0;
			data.SysMemSlicePitch = 0;
			data.pSysMem = &source;

			D3D11_BUFFER_DESC bufferDesc = {};
			bufferDesc.ByteWidth = sizeof(T);
			bufferDesc.Usage = (dynamic ? D3D11_USAGE::D3D11_USAGE_DYNAMIC : D3D11_USAGE::D3D11_USAGE_DEFAULT);
			bufferDesc.BindFlags = bindFlags;
			bufferDesc.CPUAccessFlags = (dynamic ? D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE : 0);
			bufferDesc.MiscFlags = 0;
			bufferDesc.StructureByteStride = sizeof(T);

			HRESULT hr = DXDevice::GetDevice().CreateBuffer(&bufferDesc, &data, &comptrBuffer);

			E2_ASSERT_HR(hr, "DXBuffer CreateBuffer failed");

			capacity = 1;
			elementSize = sizeof(T);
			isDynamic = dynamic;
		}

		template <typename T>
		inline void UpdateBuffer(std::vector<T>& source) { UpdateBuffer(source, source.size()); }

		template <typename T>
		void UpdateBuffer(std::vector<T>& source, size_t size) {
			E2_ASSERT(size > 0 && size <= capacity, "Update buffer size invalid");
			D3D11_MAPPED_SUBRESOURCE mappedSubResource;
			DXDevice::GetContext().Map(GetPtr(), 0u, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubResource);
			memcpy(mappedSubResource.pData, source.data(), sizeof(T) * size);
			DXDevice::GetContext().Unmap(GetPtr(), 0);
		}

		template <typename T>
		void UpdateBuffer(T& source) {
			D3D11_MAPPED_SUBRESOURCE mappedSubResource;
			DXDevice::GetContext().Map(GetPtr(), 0u, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubResource);
			memcpy(mappedSubResource.pData, &source, sizeof(T));
			DXDevice::GetContext().Unmap(GetPtr(), 0);
		}

		inline wrl::ComPtr<ID3D11Buffer>& Get() { return comptrBuffer; };
		inline ID3D11Buffer* GetPtr() { return comptrBuffer.Get(); }

	protected:
		wrl::ComPtr<ID3D11Buffer> comptrBuffer;
		size_t capacity = 0;
		size_t elementSize = 0;
		bool isDynamic = false;
	};
}