#pragma once
#include "Common.h"
#include "DXDevice.h"
#include "Instrumentation.h"

namespace Engine2
{
	class ConstantBufferBase
	{
	public:
		std::shared_ptr<ConstantBufferBase> Clone() const { return std::shared_ptr<ConstantBufferBase>(CloneImpl()); }

		inline void SetSlot(UINT newSlot) { slot = newSlot; }
		inline UINT GetSlot() { return slot; }

		void OnImgui() {}

		template <typename T>
		void Initialise() {
			//D3D11_SUBRESOURCE_DATA constBufferData = {};
			//constBufferData.SysMemPitch = 0;
			//constBufferData.SysMemSlicePitch = 0;
			//constBufferData.pSysMem = &data;

			D3D11_BUFFER_DESC bufferDesc = {};
			bufferDesc.ByteWidth = sizeof(T);
			bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
			bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			//bufferDesc.MiscFlags = 0;
			//bufferDesc.StructureByteStride = 0; // sizeof(T);

			HRESULT hr = DXDevice::GetDevice().CreateBuffer(&bufferDesc, nullptr, &pConstantBuffer);
			//HRESULT hr = DXDevice::GetDevice().CreateBuffer(&bufferDesc, &constBufferData, &pConstantBuffer);

			E2_ASSERT_HR(hr, "ConstantBuffer CreateBuffer failed");
		}

		template <typename T>
		void UpdateBuffer(T& data)
		{
			D3D11_MAPPED_SUBRESOURCE mappedSubResource;
			HRESULT hr = DXDevice::GetContext().Map(pConstantBuffer.Get(), 0u, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubResource);
			E2_ASSERT_HR(hr, "ConstantBuffer Map failed");
			memcpy(mappedSubResource.pData, &data, sizeof(T)); //*(T*)mappedSubResource.pData = data;
			DXDevice::GetContext().Unmap(pConstantBuffer.Get(), 0);
		}

	protected:
		UINT slot;
		wrl::ComPtr<ID3D11Buffer> pConstantBuffer = nullptr;

		virtual ConstantBufferBase* CloneImpl() const = 0;
	};

	template <typename T>
	class ConstantBuffer : public ConstantBufferBase
	{
	public:
		T data = {};

		ConstantBuffer(unsigned int bindSlot = 0)
		{
			SetSlot(bindSlot);
			Initialise<T>();
		}

		void UpdateBuffer()
		{
			D3D11_MAPPED_SUBRESOURCE mappedSubResource;
			HRESULT hr = DXDevice::GetContext().Map(pConstantBuffer.Get(), 0u, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubResource);
			E2_ASSERT_HR(hr, "ConstantBuffer Map failed");
			memcpy(mappedSubResource.pData, &data, sizeof(T)); //*(T*)mappedSubResource.pData = data;
			DXDevice::GetContext().Unmap(pConstantBuffer.Get(), 0);
		}

		void Bind() { throw std::exception("Use specific type of Constant buffer"); }
		void Unbind() { throw std::exception("Use specific type of Constant buffer"); }

		void PSBind()
		{
			E2_STATS_PSCB_BIND;
			this->UpdateBuffer();
			DXDevice::GetContext().PSSetConstantBuffers(this->slot, 1u, this->pConstantBuffer.GetAddressOf());
		}

		void PSUnbind() { DXDevice::GetContext().PSSetConstantBuffers(this->slot, 0u, nullptr); }

		void VSBind()
		{
			E2_STATS_VSCB_BIND;
			this->UpdateBuffer();
			DXDevice::GetContext().VSSetConstantBuffers(this->slot, 1u, this->pConstantBuffer.GetAddressOf());
		}

		void VSUnbind() { DXDevice::GetContext().VSSetConstantBuffers(this->slot, 0u, nullptr); }

		void GSBind()
		{
			E2_STATS_GSCB_BIND;
			this->UpdateBuffer();
			DXDevice::GetContext().GSSetConstantBuffers(this->slot, 1u, this->pConstantBuffer.GetAddressOf());
		}

		void GSUnbind() { DXDevice::GetContext().GSSetConstantBuffers(this->slot, 0u, nullptr); }

		void CSBind()
		{
			// To Do: instrumentation E2_STATS_CSCB_BIND;
			this->UpdateBuffer();
			DXDevice::GetContext().CSSetConstantBuffers(this->slot, 1u, this->pConstantBuffer.GetAddressOf());
		}

		void CSUnbind() { DXDevice::GetContext().CSSetConstantBuffers(this->slot, 0u, nullptr); }

	protected:
		ConstantBuffer<T>* CloneImpl() const { return new ConstantBuffer<T>(*this); }
	};

	template <typename T>
	class PSConstantBuffer : public ConstantBuffer<T>
	{
	public:
		PSConstantBuffer(unsigned int bindSlot = 0) : ConstantBuffer<T>(bindSlot) {}

		inline void Bind() { this->PSBind(); }

		inline void Unbind() { this->PSUnbind(); }

	protected:
		PSConstantBuffer<T>* CloneImpl() const { return new PSConstantBuffer(*this); }
	};

	template <typename T>
	class VSConstantBuffer : public ConstantBuffer<T>
	{
	public:
		VSConstantBuffer(unsigned int bindSlot = 0) : ConstantBuffer<T>(bindSlot) {}

		void Bind() { this->VSBind(); }

		void Unbind() { this->VSUnbind(); }

	protected:
		VSConstantBuffer<T>* CloneImpl() const { return new VSConstantBuffer(*this); }
	};

	template <typename T>
	class GSConstantBuffer : public ConstantBuffer<T>
	{
	public:
		GSConstantBuffer(unsigned int bindSlot = 0) : ConstantBuffer<T>(bindSlot) {}

		void Bind() { this->GSBind(); }

		void Unbind() { this->GSUnbind(); }

	protected:
		GSConstantBuffer<T>* CloneImpl() const { return new GSConstantBuffer(*this); }
	};

	template <typename T>
	class CSConstantBuffer : public ConstantBuffer<T>
	{
	public:
		CSConstantBuffer(unsigned int bindSlot = 0) : ConstantBuffer<T>(bindSlot) {}

		void Bind() { this->CSBind(); }

		void Unbind() { this->CSUnbind(); }

	protected:
		CSConstantBuffer<T>* CloneImpl() const { return new CSConstantBuffer(*this); }
	};

}
