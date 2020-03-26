#pragma once
#include "Common.h"
#include "Engine2.h"

namespace Engine2
{
	template <typename T>
	class ConstantBuffer : public Resource
	{
	public:
		T data = {};
		unsigned int slot = 0;

		ConstantBuffer()
		{
			D3D11_SUBRESOURCE_DATA constBufferData = {};
			//constBufferData.SysMemPitch = 0;
			//constBufferData.SysMemSlicePitch = 0;
			constBufferData.pSysMem = &data;

			D3D11_BUFFER_DESC bufferDesc = {};
			bufferDesc.ByteWidth = sizeof(T);
			bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
			bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			//bufferDesc.MiscFlags = 0;
			//bufferDesc.StructureByteStride = 0; // sizeof(T);

			HRESULT hr = Engine::GetDevice().CreateBuffer(&bufferDesc, &constBufferData, &pConstantBuffer);

			E2_ASSERT_HR(hr, "ConstantBuffer CreateBuffer failed");
		}

		~ConstantBuffer() {};

		void UpdateBuffer()
		{
			// to do: D3D11_MAP_WRITE_DISCARD v D3D11_MAP_WRITE_NO_OVERWRITE ?

			HRESULT hr;

			auto& context = Engine::GetContext();
			auto ptrBuffer = pConstantBuffer.Get();

			D3D11_MAPPED_SUBRESOURCE mappedSubResource = {};
			hr = context.Map(ptrBuffer, 0u, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubResource);

			E2_ASSERT_HR(hr, "ConstantBuffer Map failed");
			
			memcpy(mappedSubResource.pData, &data, sizeof(T)); //*(T*)mappedSubResource.pData = data;

			context.Unmap(ptrBuffer, 0);
		}

	protected:
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

	template <typename T>
	class VSConstantBuffer : public ConstantBuffer<T>
	{
	public:
		void Bind()
		{
			this->UpdateBuffer();
			Engine::GetContext().VSSetConstantBuffers(this->slot, 1u, this->pConstantBuffer.GetAddressOf());
		}
	};

}
