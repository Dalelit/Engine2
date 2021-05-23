#pragma once
#include "Common.h"
#include "DXDevice.h"

namespace Engine2
{
	class ConstantBuffer2
	{
	public:

		ConstantBuffer2() = default;

		// Note: copy constructor does not copy the contents of the buffer. Just recreates one the same size.
		ConstantBuffer2(const ConstantBuffer2& buffer) {
			slot = buffer.slot;
			sizeBytes = buffer.sizeBytes;
			if (pConstantBuffer) { Initialise(sizeBytes); }
		}

		~ConstantBuffer2() = default;

		inline void SetSlot(UINT newSlot) { slot = newSlot; }
		inline UINT GetSlot() { return slot; }

		template <typename T>
		void Initialise() {
			Initialise(sizeof(T));
		}

		template <typename T>
		void Initialise(T& data) {

			D3D11_SUBRESOURCE_DATA constBufferData = {};
			constBufferData.SysMemPitch = 0;
			constBufferData.SysMemSlicePitch = 0;
			constBufferData.pSysMem = &data;

			Initialise(sizeof(T), &constBufferData);
		}

		void Initialise(UINT size, D3D11_SUBRESOURCE_DATA* pData = nullptr)
		{
			sizeBytes = size;

			D3D11_BUFFER_DESC bufferDesc = {};
			bufferDesc.ByteWidth = sizeBytes;
			bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
			bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			//bufferDesc.MiscFlags = 0;
			//bufferDesc.StructureByteStride = 0; // sizeof(T);

			HRESULT hr = DXDevice::GetDevice().CreateBuffer(&bufferDesc, pData, &pConstantBuffer);

			E2_ASSERT_HR(hr, "ConstantBuffer CreateBuffer failed");
		}

		template <typename T>
		void UpdateBuffer(const T& data)
		{
			D3D11_MAPPED_SUBRESOURCE mappedSubResource;
			HRESULT hr = DXDevice::GetContext().Map(pConstantBuffer.Get(), 0u, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubResource);
			E2_ASSERT_HR(hr, "ConstantBuffer Map failed");
			memcpy(mappedSubResource.pData, &data, sizeof(T));
			DXDevice::GetContext().Unmap(pConstantBuffer.Get(), 0);
		}

		void PSBind()   { DXDevice::GetContext().PSSetConstantBuffers(this->slot, 1u, this->pConstantBuffer.GetAddressOf()); }
		void PSUnbind() { DXDevice::GetContext().PSSetConstantBuffers(this->slot, 0u, nullptr); }

		void VSBind()   { DXDevice::GetContext().VSSetConstantBuffers(this->slot, 1u, this->pConstantBuffer.GetAddressOf()); }
		void VSUnbind() { DXDevice::GetContext().VSSetConstantBuffers(this->slot, 0u, nullptr); }

		void GSBind()   { DXDevice::GetContext().GSSetConstantBuffers(this->slot, 1u, this->pConstantBuffer.GetAddressOf()); }
		void GSUnbind() { DXDevice::GetContext().GSSetConstantBuffers(this->slot, 0u, nullptr); }

		void CSBind()   { DXDevice::GetContext().CSSetConstantBuffers(this->slot, 1u, this->pConstantBuffer.GetAddressOf()); }
		void CSUnbind() { DXDevice::GetContext().CSSetConstantBuffers(this->slot, 0u, nullptr); }

	protected:
		UINT slot = 0;
		UINT sizeBytes = 0;
		wrl::ComPtr<ID3D11Buffer> pConstantBuffer = nullptr;
	};

}
