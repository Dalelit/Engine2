#pragma once
#include "Common.h"
#include "DXDevice.h"
#include "Resources.h"
#include "Instrumentation.h"

namespace Engine2
{
	class ConstantBufferBase : public Bindable
	{
	public:
		std::shared_ptr<ConstantBufferBase> Clone() const { return std::shared_ptr<ConstantBufferBase>(CloneImpl()); }

	protected:
		virtual ConstantBufferBase* CloneImpl() const = 0;
	};

	template <typename T>
	class ConstantBuffer : public ConstantBufferBase
	{
	public:
		T data = {};
		unsigned int slot;

		ConstantBuffer(unsigned int bindSlot) :
			slot(bindSlot)
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

			HRESULT hr = DXDevice::GetDevice().CreateBuffer(&bufferDesc, &constBufferData, &pConstantBuffer);

			E2_ASSERT_HR(hr, "ConstantBuffer CreateBuffer failed");
		}

		void UpdateBuffer()
		{
			D3D11_MAPPED_SUBRESOURCE mappedSubResource;
			HRESULT hr = DXDevice::GetContext().Map(pConstantBuffer.Get(), 0u, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubResource);
			E2_ASSERT_HR(hr, "ConstantBuffer Map failed");
			memcpy(mappedSubResource.pData, &data, sizeof(T)); //*(T*)mappedSubResource.pData = data;
			DXDevice::GetContext().Unmap(pConstantBuffer.Get(), 0);
		}

	protected:
		wrl::ComPtr<ID3D11Buffer> pConstantBuffer = nullptr;
	};

	template <typename T>
	class PSConstantBuffer : public ConstantBuffer<T>
	{
	public:
		PSConstantBuffer(unsigned int bindSlot = 0) : ConstantBuffer<T>(bindSlot) {}

		void Bind()
		{
			E2_STATS_PSCB_BIND;
			this->UpdateBuffer();
			DXDevice::GetContext().PSSetConstantBuffers(this->slot, 1u, this->pConstantBuffer.GetAddressOf());
		}

		void Unbind() { DXDevice::GetContext().PSSetConstantBuffers(this->slot, 0u, nullptr); }

		void OnImgui()
		{
			if (ImGui::TreeNode("Pixel Shader Constant buffer"))
			{
				ImGui::Text("No override");
				ImGui::TreePop();
			}
		}

	protected:
		PSConstantBuffer<T>* CloneImpl() const { return new PSConstantBuffer(*this); }
	};

	template <typename T>
	class VSConstantBuffer : public ConstantBuffer<T>
	{
	public:
		VSConstantBuffer(unsigned int bindSlot = 0) : ConstantBuffer<T>(bindSlot) {}

		void Bind()
		{
			E2_STATS_VSCB_BIND;
			this->UpdateBuffer();
			DXDevice::GetContext().VSSetConstantBuffers(this->slot, 1u, this->pConstantBuffer.GetAddressOf());
		}

		void Unbind() { DXDevice::GetContext().VSSetConstantBuffers(this->slot, 0u, nullptr); }

		void OnImgui()
		{
			if (ImGui::TreeNode("Vertex Shader Constant buffer"))
			{
				ImGui::Text("No override");
				ImGui::TreePop();
			}
		}

	protected:
		VSConstantBuffer<T>* CloneImpl() const { return new VSConstantBuffer(*this); }
	};

	template <typename T>
	class GSConstantBuffer : public ConstantBuffer<T>
	{
	public:
		GSConstantBuffer(unsigned int bindSlot = 0) : ConstantBuffer<T>(bindSlot) {}

		void Bind()
		{
			E2_STATS_GSCB_BIND;
			this->UpdateBuffer();
			DXDevice::GetContext().GSSetConstantBuffers(this->slot, 1u, this->pConstantBuffer.GetAddressOf());
		}

		void Unbind() { DXDevice::GetContext().GSSetConstantBuffers(this->slot, 0u, nullptr); }

	protected:
		GSConstantBuffer<T>* CloneImpl() const { return new GSConstantBuffer(*this); }
	};

}
