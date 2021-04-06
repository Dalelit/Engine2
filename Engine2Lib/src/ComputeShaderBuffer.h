#pragma once
#include "Common.h"
#include "DXDevice.h"
#include "DXBuffer.h"
#include "Resources.h"

namespace Engine2
{
	class ComputeShaderSRV : public Bindable
	{
	public:
		void Bind() {
			DXDevice::GetContext().CSSetShaderResources(slot, 1, pShaderResourceView.GetAddressOf());
		}

		void Unbind() {
			ID3D11ShaderResourceView* pSRVNULL = nullptr;
			DXDevice::GetContext().CSSetShaderResources(slot, 1, &pSRVNULL);
		}

		void OnImgui() { ImGui::Text("ComputeShaderSRV"); }

		void Initialise(wrl::ComPtr<ID3D11ShaderResourceView> pSRV) {
			pShaderResourceView = pSRV;
		}

	protected:
		UINT slot = 0;

		wrl::ComPtr<ID3D11ShaderResourceView> pShaderResourceView = nullptr;
	};

	class ComputeShaderUAV : public Bindable
	{
	public:
		void Bind() {
			DXDevice::GetContext().CSSetUnorderedAccessViews(slot, 1, pUnorderedAccessView.GetAddressOf(), NULL);
		}

		void Unbind() {
			ID3D11UnorderedAccessView* pUAVNULL = nullptr;
			DXDevice::GetContext().CSSetUnorderedAccessViews(slot, 1, &pUAVNULL, NULL);
		}

		inline void SetSlot(UINT newSlot) { slot = newSlot; }

		void OnImgui() { ImGui::Text("ComputeShaderUAV"); }

		void Initialise(wrl::ComPtr<ID3D11Texture2D> pTargetBuffer) {

			D3D11_UNORDERED_ACCESS_VIEW_DESC viewDesc = {};

			viewDesc.ViewDimension = D3D11_UAV_DIMENSION::D3D11_UAV_DIMENSION_TEXTURE2D;
			viewDesc.Format = DXGI_FORMAT::DXGI_FORMAT_UNKNOWN;
			viewDesc.Texture2D.MipSlice = 0;

			HRESULT hr = DXDevice::GetDevice().CreateUnorderedAccessView(pTargetBuffer.Get(), &viewDesc, &pUnorderedAccessView);

			E2_ASSERT_HR(hr, "Computer Shader output view CreateUnorderedAccessView failed");
		}

		void InitialiseTest(wrl::ComPtr<ID3D11Buffer> pTargetBuffer, UINT capacity) {

			D3D11_UNORDERED_ACCESS_VIEW_DESC viewDesc = {};

			viewDesc.ViewDimension = D3D11_UAV_DIMENSION::D3D11_UAV_DIMENSION_BUFFER;
			viewDesc.Format = DXGI_FORMAT::DXGI_FORMAT_UNKNOWN;
			viewDesc.Buffer.FirstElement = 0;
			viewDesc.Buffer.NumElements = capacity;

			HRESULT hr = DXDevice::GetDevice().CreateUnorderedAccessView(pTargetBuffer.Get(), &viewDesc, &pUnorderedAccessView);

			E2_ASSERT_HR(hr, "Computer Shader output view CreateUnorderedAccessView failed");
		}

	protected:
		UINT slot = 0;

		wrl::ComPtr<ID3D11UnorderedAccessView> pUnorderedAccessView = nullptr;
	};

}