#pragma once
#include "pch.h"
#include "Engine2.h"
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
		Texture(unsigned int slot, Surface& surface, DXGI_FORMAT format);
		~Texture() = default;

		void Bind();

		void SetSampler(std::shared_ptr<TextureSampler> pTexSampler) { pSampler = pTexSampler; }

		void OnImgui();

	protected:
		unsigned int slot;
		std::string name = "Texture";

		std::shared_ptr<TextureSampler> pSampler = nullptr;

		Microsoft::WRL::ComPtr<ID3D11Texture2D1> pTexture = nullptr;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pSRView = nullptr;
	};
}