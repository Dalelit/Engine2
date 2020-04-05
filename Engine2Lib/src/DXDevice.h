#pragma once
#include "pch.h"

namespace wrl = Microsoft::WRL;

namespace Engine2
{
	class RenderTarget
	{
	public:
		wrl::ComPtr<ID3D11Texture2D> pBuffer = nullptr;
		wrl::ComPtr<ID3D11RenderTargetView> pTargetView = nullptr;
		wrl::ComPtr<ID3D11ShaderResourceView> pResourceView = nullptr;
		wrl::ComPtr<ID3D11SamplerState> pSamplerState = nullptr;
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

		unsigned int CreateOffscreenRenderTarget();
		void BindRenderTargetAsTarget(unsigned int id, bool useDepthBuffer = false);
		void BindBackbufferRenderTarget() { BindRenderTargetAsTarget(0, true); };

		void BindRenderTargetAsResource(unsigned int id, unsigned int slot);
		void UnbindRenderTargetAsResource(unsigned int id, unsigned int slot);

		void LogDebugInfo();

	protected:
		HWND hwnd;

		float clearColor[4] = { 0.0f,0.0f,0.0f,1.0f };

		wrl::ComPtr<IDXGISwapChain1> pSwapChain = nullptr;
		wrl::ComPtr<ID3D11Device3> pDevice = nullptr;
		wrl::ComPtr<ID3D11DeviceContext3> pImmediateContext = nullptr;
		wrl::ComPtr<ID3D11DepthStencilView> pDepthStencilView = nullptr;
		wrl::ComPtr<ID3D11Texture2D> pDepthTexture = nullptr;
		wrl::ComPtr<ID3D11DepthStencilState> pDepthStencilStateOn = nullptr;
		D3D11_TEXTURE2D_DESC backBufferDesc;

		void CreateDeviceAndSwapchain();
		void SetBackBuffer();
		void ConfigureDepthBuffer();
		void ConfigurePipeline();
		void ReleasePipeline();

		void ConfigureRenderTarget(RenderTarget& rt);

		std::vector<RenderTarget> renderTargets;
	};

}