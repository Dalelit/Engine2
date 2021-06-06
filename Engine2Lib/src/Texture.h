#pragma once
#include "pch.h"
#include "DXDevice.h"
#include "Surface.h"

namespace Engine2
{
	class TextureSampler
	{
	public:
		TextureSampler(D3D11_FILTER filter, D3D11_TEXTURE_ADDRESS_MODE addressMode);
		~TextureSampler() = default;

		Microsoft::WRL::ComPtr<ID3D11SamplerState>& GetSamplerState() { return pSamplerState; }

	protected:
		Microsoft::WRL::ComPtr<ID3D11SamplerState> pSamplerState = nullptr;
	};

	class Texture
	{
	public:
		Texture(unsigned int slot, Surface& surface, DXGI_FORMAT format, bool unorderedAccess = false);
		Texture(unsigned int slot, Surface& surface, D3D11_TEXTURE2D_DESC1& texDesc, D3D11_SHADER_RESOURCE_VIEW_DESC& srvDesc);
		Texture(unsigned int slot, std::vector<std::shared_ptr<Surface>> surfaces, D3D11_TEXTURE2D_DESC1& texDesc, D3D11_SHADER_RESOURCE_VIEW_DESC& srvDesc);
		Texture(unsigned int slot, Microsoft::WRL::ComPtr<ID3D11Texture2D> pTextureToWrap);
		~Texture() = default;

		void Bind();
		void Unbind();

		void SetSampler(D3D11_FILTER filter, D3D11_TEXTURE_ADDRESS_MODE addressMode) { pSampler = std::make_shared<TextureSampler>(filter, addressMode); }
		void SetSampler(std::shared_ptr<TextureSampler> pTexSampler) { pSampler = pTexSampler; }

		void OnImgui();

		void SetName(const std::string& newName) { name = newName; }
		void SetSlot(unsigned int newSlot) { slot = newSlot; }

		inline Microsoft::WRL::ComPtr<ID3D11Texture2D1> GetBuffer() { return pTexture; }
		inline Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> GetSRV() { return pSRView; }

	protected:
		unsigned int slot;
		std::string name;
		std::string info;

		std::shared_ptr<TextureSampler> pSampler = nullptr;

		Microsoft::WRL::ComPtr<ID3D11Texture2D1> pTexture = nullptr;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pSRView = nullptr;

		void Initialise(D3D11_TEXTURE2D_DESC1& texDesc, D3D11_SHADER_RESOURCE_VIEW_DESC& srvDesc, D3D11_SUBRESOURCE_DATA* data);
	};
}
