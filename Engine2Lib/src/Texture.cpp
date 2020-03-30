#include "pch.h"
#include "Texture.h"

namespace Engine2
{
	Texture::Texture(unsigned int slot, Surface& surface, DXGI_FORMAT format) :
		slot(slot)
	{
		HRESULT hr;

		D3D11_TEXTURE2D_DESC1 texDesc = {};
		texDesc.Width = surface.GetWidth();
		texDesc.Height = surface.GetHeight();
		texDesc.MipLevels = 1u;
		texDesc.ArraySize = 1u;
		texDesc.Format = format;
		texDesc.SampleDesc.Count = 1u;
		texDesc.SampleDesc.Quality = 0u;
		texDesc.Usage = D3D11_USAGE_DEFAULT;
		texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		//texDesc.CPUAccessFlags;
		//texDesc.TextureLayout = D3D11_TEXTURE_LAYOUT_UNDEFINED;

		D3D11_SUBRESOURCE_DATA data = {};
		data.pSysMem = surface.GetDataVoidPtr();
		data.SysMemPitch = surface.GetPitch();
		data.SysMemSlicePitch = surface.GetSlicePitch();

		hr = Engine::GetDevice().CreateTexture2D1(&texDesc, &data, &pTexture);

		E2_ASSERT_HR(hr, "CreateTexture2D1 failed");

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = texDesc.Format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = 1;

		hr = Engine::GetDevice().CreateShaderResourceView(pTexture.Get(), &srvDesc, &pSRView);

		E2_ASSERT_HR(hr, "CreateShaderResourceView failed");
	}

	void Texture::Bind()
	{
		Engine::GetContext().PSSetShaderResources(slot, 1u, pSRView.GetAddressOf());
		Engine::GetContext().PSSetSamplers(slot, 1u, pSampler->GetSamplerState().GetAddressOf());
	}

	void Texture::OnImgui()
	{
		if (ImGui::TreeNode(name.c_str()))
		{
			ImGui::Text("Slot %i", slot);
			ImGui::TreePop();
		}
	}

	TextureSampler::TextureSampler(D3D11_FILTER filter, D3D11_TEXTURE_ADDRESS_MODE addressMode)
	{
		D3D11_SAMPLER_DESC sampDesc = {};
		sampDesc.Filter = filter;
		sampDesc.AddressU = addressMode;
		sampDesc.AddressV = addressMode;
		sampDesc.AddressW = addressMode;

		HRESULT hr = Engine::GetDevice().CreateSamplerState(&sampDesc, &pSamplerState);

		E2_ASSERT_HR(hr, "CreateSamplerState failed");
	}
}
