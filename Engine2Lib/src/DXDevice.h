#pragma once
#include "pch.h"

namespace wrl = Microsoft::WRL;

namespace Engine2
{
	class Resource
	{
	public:
		virtual void Bind() = 0;
	};

	typedef std::vector<std::shared_ptr<Resource>> ResourceVector;

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

		void LogDebugInfo();

	protected:
		HWND hwnd;

		float clearColor[4] = { 0.0f,0.0f,0.0f,1.0f };

		wrl::ComPtr<IDXGISwapChain1> pSwapChain = nullptr;
		wrl::ComPtr<ID3D11Device3> pDevice = nullptr;
		wrl::ComPtr<ID3D11DeviceContext3> pImmediateContext = nullptr;
		D3D11_TEXTURE2D_DESC bufferDesc;
		wrl::ComPtr<ID3D11Texture2D> pBackBuffer = nullptr;
		wrl::ComPtr<ID3D11RenderTargetView> pRenderTargetView = nullptr;
		wrl::ComPtr<ID3D11DepthStencilView> pDepthStencilView = nullptr;
		wrl::ComPtr<ID3D11Texture2D> pDepthTexture = nullptr;
		wrl::ComPtr<ID3D11DepthStencilState> pDepthStencilStateOn = nullptr;

		void CreateDeviceAndSwapchain();
		void ConfigurePipeline();
		void ReleasePipeline();
	};

}