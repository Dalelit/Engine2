#pragma once
#include "DXDevice.h"
#include "TextureGizmo.h"

namespace Engine2
{
	class ShadowMap
	{
	public:
		ShadowMap();

		void Initialise(UINT bufferWidth, UINT bufferHeight);

		void Clear();
		void SetAsTarget();

		// binds the shadow map as a shader resource
		void BindWtihComparisonSampler();
		void BindWithSimpleSampler();

		void SetSlot(unsigned int textureSlot) { slot = textureSlot; }

		void OnImgui();

	protected:
		unsigned int slot = 1;

		UINT stencilRef = 0;
		wrl::ComPtr<ID3D11Texture2D> pDepthBuffer = nullptr;
		wrl::ComPtr<ID3D11DepthStencilView> pDepthStencilView = nullptr;
		wrl::ComPtr<ID3D11DepthStencilState> pDepthStencilState = nullptr;
		wrl::ComPtr<ID3D11ShaderResourceView> pDepthBufferResourceView = nullptr;
		wrl::ComPtr<ID3D11SamplerState> pDepthBufferSamplerState = nullptr;
		wrl::ComPtr<ID3D11SamplerState> pDepthBufferSamplerComparisonState = nullptr;

		// set when the buffer is created so it can be used by other resources
		UINT width;
		UINT height;
		D3D11_VIEWPORT viewport;

		std::unique_ptr<TextureGizmo> pTexGizmo;

		void Initialise();
	};
}