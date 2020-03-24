#pragma once
#include "Common.h"
#include "Engine2.h"

namespace Engine2
{
	template <typename T>
	class ConstantBuffer : public Resource
	{
	public:
		T data;
		unsigned int slot = 0;

		ConstantBuffer()
		{
			constBufferData.SysMemPitch = 0;
			constBufferData.SysMemSlicePitch = 0;
			constBufferData.pSysMem = &data;

			bufferDesc.ByteWidth = sizeof(T);
			bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
			bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			bufferDesc.MiscFlags = 0;
			bufferDesc.StructureByteStride = sizeof(T);
		}

		~ConstantBuffer() {};

		void UpdateBuffer()
		{
			HRESULT hr = Engine::GetDevice().CreateBuffer(&bufferDesc, &constBufferData, &pConstantBuffer);

			E2_ASSERT_HR(hr, "ConstantBuffer CreateBuffer failed");

			// to do: update rather than create each time
			//Engine::GetContext().UpdateSubresource(pConstantBuffer.Get(), 0u, nullptr, constBufferData.pSysMem, constBufferData.SysMemPitch, constBufferData.SysMemSlicePitch);
		}

	protected:
		D3D11_SUBRESOURCE_DATA constBufferData = {};
		D3D11_BUFFER_DESC bufferDesc = {};
		wrl::ComPtr<ID3D11Buffer> pConstantBuffer = nullptr;
	};

	template <typename T>
	class PSConstantBuffer : public ConstantBuffer<T>
	{
	public:
		void Bind()
		{
			this->UpdateBuffer();
			Engine::GetContext().PSSetConstantBuffers(this->slot, 1u, this->pConstantBuffer.GetAddressOf());
		}
	};

}
