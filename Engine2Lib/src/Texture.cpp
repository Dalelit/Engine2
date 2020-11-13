#include "pch.h"
#include "Texture.h"

namespace Engine2
{
	Texture::Texture(unsigned int slot, Surface& surface, DXGI_FORMAT format) :
		slot(slot)
	{
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

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = texDesc.Format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION::D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = 1;

		Initialise(texDesc, srvDesc, &data);
	}

	Texture::Texture(unsigned int slot, Surface& surface, D3D11_TEXTURE2D_DESC1& texDesc, D3D11_SHADER_RESOURCE_VIEW_DESC& srvDesc) :
		slot(slot)
	{
		D3D11_SUBRESOURCE_DATA data = {};
		data.pSysMem = surface.GetDataVoidPtr();
		data.SysMemPitch = surface.GetPitch();
		data.SysMemSlicePitch = surface.GetSlicePitch();

		Initialise(texDesc, srvDesc, &data);
	}

	Texture::Texture(unsigned int slot, std::vector<std::shared_ptr<Surface>> surfaces, D3D11_TEXTURE2D_DESC1& texDesc, D3D11_SHADER_RESOURCE_VIEW_DESC& srvDesc) :
		slot(slot)
	{
		E2_ASSERT_HR(texDesc.ArraySize == surfaces.size(), "Texture Desc array size does not match surface count");

		D3D11_SUBRESOURCE_DATA data[6] = {};
		for (int i = 0; i < 6; i++)
		{
			data[i].pSysMem = surfaces[i]->GetDataVoidPtr();
			data[i].SysMemPitch = surfaces[i]->GetPitch();
			data[i].SysMemSlicePitch = surfaces[i]->GetSlicePitch();
		}

		Initialise(texDesc, srvDesc, data);
	}

	Texture::Texture(unsigned int slot, Microsoft::WRL::ComPtr<ID3D11Texture2D> pTextureToWrap) :
		slot(slot)
	{
		HRESULT hr;

		hr = pTextureToWrap.As(&pTexture);

		E2_ASSERT_HR(hr, "Texture as ver 1 failed");

		D3D11_TEXTURE2D_DESC1 texDesc;
		pTexture->GetDesc1(&texDesc);

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = texDesc.Format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION::D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = 1;

		hr = DXDevice::GetDevice().CreateShaderResourceView(pTexture.Get(), &srvDesc, &pSRView);

		E2_ASSERT_HR(hr, "CreateShaderResourceView failed");

		name = "Texture2D wrap existing texture";
		info = "MipsLevels " + std::to_string(srvDesc.Texture2D.MipLevels);
	}

	void Texture::Initialise(D3D11_TEXTURE2D_DESC1& texDesc, D3D11_SHADER_RESOURCE_VIEW_DESC& srvDesc, D3D11_SUBRESOURCE_DATA* data)
	{
		HRESULT hr;

		hr = DXDevice::GetDevice().CreateTexture2D1(&texDesc, data, &pTexture);

		E2_ASSERT_HR(hr, "CreateTexture2D1 failed");

		hr = DXDevice::GetDevice().CreateShaderResourceView(pTexture.Get(), &srvDesc, &pSRView);

		E2_ASSERT_HR(hr, "CreateShaderResourceView failed");

		name = "Texture2D";
		info = "Size " + std::to_string(texDesc.Width) + "x" + std::to_string(texDesc.Height) + " MipsLevels " + std::to_string(srvDesc.Texture2D.MipLevels);
	}

	void Texture::Bind()
	{
		DXDevice::GetContext().PSSetShaderResources(slot, 1u, pSRView.GetAddressOf());
		DXDevice::GetContext().PSSetSamplers(slot, 1u, pSampler->GetSamplerState().GetAddressOf());
	}

	void Texture::Unbind()
	{
		ID3D11ShaderResourceView* const srv[1] = { nullptr };
		DXDevice::GetContext().PSSetShaderResources(slot, 1u, srv);
		DXDevice::GetContext().PSSetSamplers(slot, 0u, nullptr);
	}

	void Texture::OnImgui()
	{
		if (ImGui::TreeNode(name.c_str()))
		{
			ImGui::Text("Slot %i", slot);
			ImGui::Text(info.c_str());
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

		HRESULT hr = DXDevice::GetDevice().CreateSamplerState(&sampDesc, &pSamplerState);

		E2_ASSERT_HR(hr, "CreateSamplerState failed");
	}
}
