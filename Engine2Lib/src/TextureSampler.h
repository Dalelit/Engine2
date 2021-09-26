#pragma once
#include "DXDevice.h"

namespace Engine2
{
	class TextureSampler
	{
	public:
		TextureSampler(D3D11_FILTER filter, D3D11_TEXTURE_ADDRESS_MODE addressMode);
		~TextureSampler() = default;

		Microsoft::WRL::ComPtr<ID3D11SamplerState>& GetSamplerState() { return pSamplerState; }

		void OnImgui();

		static std::shared_ptr<TextureSampler> CreateLinearClamp() { return std::make_shared<TextureSampler>(D3D11_FILTER::D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_CLAMP); }
		static std::shared_ptr<TextureSampler> CreateNearestClamp() { return std::make_shared<TextureSampler>(D3D11_FILTER::D3D11_FILTER_MIN_MAG_MIP_POINT, D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_CLAMP); }
		static std::shared_ptr<TextureSampler> CreateDefault() { return CreateLinearClamp(); }

	protected:
		Microsoft::WRL::ComPtr<ID3D11SamplerState> pSamplerState = nullptr;
		D3D11_SAMPLER_DESC sampDesc = {};
	};
}