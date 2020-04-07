#pragma once
#include "pch.h"

namespace wrl = Microsoft::WRL;

namespace Engine2
{
	struct RenderTarget
	{
	public:
		wrl::ComPtr<ID3D11Texture2D> pBuffer = nullptr;
		wrl::ComPtr<ID3D11RenderTargetView> pTargetView = nullptr;
		wrl::ComPtr<ID3D11ShaderResourceView> pResourceView = nullptr;
		wrl::ComPtr<ID3D11SamplerState> pSamplerState = nullptr;
		wrl::ComPtr<ID3D11DepthStencilView> pDepthStencilView = nullptr;
		wrl::ComPtr<ID3D11Texture2D> pDepthTexture = nullptr;
		wrl::ComPtr<ID3D11DepthStencilState> pDepthStencilState = nullptr;
		bool depthBuffer = true;

		void Reset() {
			pBuffer.Reset();
			pTargetView.Reset();
			pResourceView.Reset();
			pSamplerState.Reset();
			pDepthStencilView.Reset();
			pDepthTexture.Reset();
			pDepthStencilState.Reset();
		}
	};

	class DXDevice
	{
	public:
		DXDevice(HWND hwnd);
		~DXDevice() = default;

		void BeginFrame();
		void PresentFrame();

		void ScreenSizeChanged();

		ID3D11Device3& GetDevice() { return *pDevice.Get(); }
		ID3D11DeviceContext3& GetContext() { return *pImmediateContext.Get(); }

		float GetAspectRatio() { return (float)backBufferDesc.Width / (float)backBufferDesc.Height; }

		unsigned int CreateOffscreenRenderTarget(bool withDepthBuffer = true);

		void BindRenderTargetAsTarget(unsigned int id);
		void BindRenderTargetAsResource(unsigned int id, unsigned int slot);
		void UnbindRenderTargetAsResource(unsigned int id, unsigned int slot);

		unsigned int GetBackbufferRenderTargetId() { return 0; } // do this way in case the number should change for some reason
		void BindBackbufferRenderTarget() { BindRenderTargetAsTarget(0); };

		void LogDebugInfo();

	protected:
		HWND hwnd;

		float clearColor[4] = { 0.0f,0.0f,0.0f,1.0f };

		wrl::ComPtr<IDXGISwapChain1> pSwapChain = nullptr;
		wrl::ComPtr<ID3D11Device3> pDevice = nullptr;
		wrl::ComPtr<ID3D11DeviceContext3> pImmediateContext = nullptr;
		D3D11_TEXTURE2D_DESC backBufferDesc;

		void CreateDeviceAndSwapchain();
		void SetBackBuffer();
		void ConfigurePipeline();
		void ReleasePipeline();
		void ConfigureRenderTarget(unsigned int indx);
		void ConfigureDepthBuffer(unsigned int indx);

		// back buffer is always at index 0 for the render targets
		std::vector<RenderTarget> renderTargets;
	};

}