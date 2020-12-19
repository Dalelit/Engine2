#include "pch.h"
#include "ShadowMap.h"

namespace Engine2
{
	ShadowMap::ShadowMap() :
		width(2048), height(2048)
	{
	}

	void ShadowMap::Initialise(UINT bufferWidth, UINT bufferHeight)
	{
		width = bufferWidth;
		height = bufferHeight;
		Initialise();
	}

	void ShadowMap::Clear()
	{
		constexpr float clearColor[4] = { 0.0f,0.0f,0.0f,0.0f };
		DXDevice::GetContext().ClearDepthStencilView(pDepthStencilView.Get(), D3D11_CLEAR_FLAG::D3D11_CLEAR_DEPTH, 1.0f, 0u);
	}

	void ShadowMap::SetAsTarget()
	{
		DXDevice::GetContext().OMSetRenderTargets(0u, nullptr, pDepthStencilView.Get());
		DXDevice::GetContext().OMSetDepthStencilState(pDepthStencilState.Get(), stencilRef);
		DXDevice::GetContext().RSSetViewports(1, &viewport);
	}

	void ShadowMap::BindWtihComparisonSampler()
	{
		DXDevice::GetContext().PSSetShaderResources(slot, 1u, pDepthBufferResourceView.GetAddressOf());
		DXDevice::GetContext().PSSetSamplers(slot, 1u, pDepthBufferSamplerComparisonState.GetAddressOf());
	}

	void ShadowMap::BindWithSimpleSampler()
	{
		DXDevice::GetContext().PSSetShaderResources(slot, 1u, pDepthBufferResourceView.GetAddressOf());
		DXDevice::GetContext().PSSetSamplers(slot, 1u, pDepthBufferSamplerState.GetAddressOf());
	}

	void ShadowMap::OnImgui()
	{
		ImGui::Text("Buffer size %i x %i", width, height);

		if (pTexGizmo)
		{
			BindWithSimpleSampler();
			pTexGizmo->OnImgui();
		}
		else
		{
			if (ImGui::Button("Enable texture gizmo")) pTexGizmo = std::make_unique<TextureGizmo>(slot);
		}
	}

	void ShadowMap::Initialise()
	{
		HRESULT hr;

		D3D11_DEPTH_STENCIL_DESC depthStencilDesc = CD3D11_DEPTH_STENCIL_DESC(CD3D11_DEFAULT());

		hr = DXDevice::GetDevice().CreateDepthStencilState(&depthStencilDesc, &pDepthStencilState);

		E2_ASSERT_HR(hr, "CreateDepthStencilState failed for DepthStencil");

		D3D11_TEXTURE2D_DESC dtDesc = {};
		dtDesc.Width = width;
		dtDesc.Height = height;
		dtDesc.MipLevels = 1u;
		dtDesc.ArraySize = 1u;
		dtDesc.Format = DXGI_FORMAT::DXGI_FORMAT_R32_TYPELESS; // DXGI_FORMAT::DXGI_FORMAT_D32_FLOAT - cannot use this and bind as a shader resource
		dtDesc.SampleDesc.Count = 1u;
		dtDesc.SampleDesc.Quality = 0u;
		dtDesc.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
		dtDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE; // so we can also draw it
		dtDesc.CPUAccessFlags = 0u;
		dtDesc.MiscFlags = 0u;

		hr = DXDevice::GetDevice().CreateTexture2D(&dtDesc, nullptr, &pDepthBuffer);

		E2_ASSERT_HR(hr, "CreateTexture2D failed for DepthStencil");

		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;
		viewport.Width = (float)width;
		viewport.Height = (float)height;
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;

		D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
		dsvDesc.Format = DXGI_FORMAT::DXGI_FORMAT_D32_FLOAT; // dtDesc.Format - use the correct format rather than typeless
		dsvDesc.ViewDimension = D3D11_DSV_DIMENSION::D3D11_DSV_DIMENSION_TEXTURE2D;
		dsvDesc.Flags = 0;
		dsvDesc.Texture2D.MipSlice = 0u;

		hr = DXDevice::GetDevice().CreateDepthStencilView(pDepthBuffer.Get(), &dsvDesc, &pDepthStencilView);

		E2_ASSERT_HR(hr, "CreateDepthStencilView failed for DepthStencil");

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = DXGI_FORMAT::DXGI_FORMAT_R32_FLOAT;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION::D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = 1;

		hr = DXDevice::GetDevice().CreateShaderResourceView(pDepthBuffer.Get(), &srvDesc, &pDepthBufferResourceView);

		E2_ASSERT_HR(hr, "CreateShaderResourceView failed for depth buffer");

		D3D11_SAMPLER_DESC sampDesc = {};
		sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_BORDER;
		sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_BORDER;
		sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_BORDER;

		sampDesc.Filter = D3D11_FILTER::D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		hr = DXDevice::GetDevice().CreateSamplerState(&sampDesc, &pDepthBufferSamplerState);

		E2_ASSERT_HR(hr, "CreateSamplerState failed");

		sampDesc.Filter = D3D11_FILTER::D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
		sampDesc.ComparisonFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL;
		hr = DXDevice::GetDevice().CreateSamplerState(&sampDesc, &pDepthBufferSamplerComparisonState);

		E2_ASSERT_HR(hr, "CreateSamplerState failed");
	}
}