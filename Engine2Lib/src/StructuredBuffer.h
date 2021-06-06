#pragma once
#include "DXBuffer.h"

namespace Engine2
{
	template <typename T>
	class StructuredBuffer
	{
	public:
		
		virtual ~StructuredBuffer() = default;

		void Initialise(std::vector<T>& source, bool dynamic = false) {
			mBuffer.InitBuffer(source, dynamic, D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE, D3D11_RESOURCE_MISC_FLAG::D3D11_RESOURCE_MISC_BUFFER_STRUCTURED);
			CreateSRV(source.size());
		}

		void Initialise(size_t capacity, bool dynamic = false) {
			mBuffer.InitBuffer<T>(capacity, dynamic, D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE, D3D11_RESOURCE_MISC_FLAG::D3D11_RESOURCE_MISC_BUFFER_STRUCTURED);
			CreateSRV(capacity);
		}

		inline void Update(std::vector<T>& source) { mBuffer.UpdateBuffer(source); }
		inline void Update(T* data, size_t size) { mBuffer.UpdateBuffer(data, size); }

		inline void BindVS() { BindVS(mSlot); }
		inline void BindPS() { BindPS(mSlot); }

		inline void BindVS(UINT slot) { DXDevice::GetContext().VSSetShaderResources(slot, 1, mpSRV.GetAddressOf()); }
		inline void BindPS(UINT slot) { DXDevice::GetContext().PSSetShaderResources(slot, 1, mpSRV.GetAddressOf()); }

	protected:
		DXBuffer mBuffer;
		wrl::ComPtr<ID3D11ShaderResourceView> mpSRV;
		UINT mSlot = 0;

		void CreateSRV(size_t capacity) {
			D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
			srvDesc.Format = DXGI_FORMAT::DXGI_FORMAT_UNKNOWN;
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION::D3D11_SRV_DIMENSION_BUFFER;
			srvDesc.BufferEx.FirstElement = 0;
			srvDesc.BufferEx.NumElements = (UINT)capacity;
			srvDesc.BufferEx.Flags = 0;

			HRESULT hr = DXDevice::GetDevice().CreateShaderResourceView(mBuffer.GetPtr(), &srvDesc, &mpSRV);
		}
	};

	template <typename T>
	class StructuredBufferLocalData : public StructuredBuffer<T>
	{
	public:
		~StructuredBufferLocalData() {
			FreeDataStore();
		}

		void Initialise(size_t capacity) {
			CreateDataStore(capacity);
			StructuredBuffer<T>::Initialise(capacity, true);
		}

		inline void Update(size_t count) { StructuredBuffer<T>::Update(data, count); }

		void CheckGrowCapacity(size_t newCapacity) {
			if (this->mBuffer.GetCapacity() < newCapacity) Resize(newCapacity);
		}

		void Resize(size_t capacity) {
			// for now this is the same as initialise, but doing it as a separate function incase I need to do it differently
			Initialise(capacity);
		}

		T* begin() { return data; }
		T* end() { return data + this->mBuffer.GetCapacity(); }

		T& operator[](int indx) {
			E2_ASSERT(indx < this->mBuffer.GetCapacity(), "Index out of range");
			return *(data + indx);
		}

	protected:
		T* data = nullptr;

		void FreeDataStore() {
			if (data)
			{
				delete[] data;
				data = nullptr;
			}
		}

		void CreateDataStore(size_t capacity) {
			E2_ASSERT(capacity > 0, "Resizing to zero");
			FreeDataStore();
			data = new T[capacity];
		}
	};
}