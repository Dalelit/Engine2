#pragma once
#include "pch.h"

namespace wrl = Microsoft::WRL;

namespace Engine2
{
	class DXDevice
	{
	public:
		static DXDevice& CreateDevice(HWND hwnd);
		inline static DXDevice& Get() { return *instance; }
		inline static ID3D11Device3& GetDevice() { return *instance->pDevice.Get(); }
		inline static ID3D11DeviceContext3& GetContext() { return *instance->pImmediateContext.Get(); }

		DXDevice(HWND hwnd);
		~DXDevice() = default;

		void BeginFrame();
		void PresentFrame();

		void ScreenSizeChanged();

		inline float GetAspectRatio() { return (float)backBufferDesc.Width / (float)backBufferDesc.Height; }
		inline D3D11_TEXTURE2D_DESC& GetBackBufferTextureDesc() { return backBufferDesc; }

		void SetBackBufferAsRenderTarget();
		void SetBackBufferAsRenderTargetNoDepthCheck();

		inline void SetDefaultRenderState()       { pImmediateContext->RSSetState(pRSDefault.Get()); }
		inline void SetWireframeRenderState()     { pImmediateContext->RSSetState(pRSWireframe.Get()); }
		inline void SetNoFaceCullingRenderState() { pImmediateContext->RSSetState(pRSNoFaceCulling.Get()); }
		inline void SetDefaultDepthStencilState() { pImmediateContext->OMSetDepthStencilState(pDepthStencilState.Get(), 0); }
		inline void SetDefaultBlendState()        { pImmediateContext->OMSetBlendState(nullptr, nullptr, 0xffffffff); }
		inline void SetAlphaBlendState()          { pImmediateContext->OMSetBlendState(pAlphaBlendState.Get(), nullptr, 0xffffffff); }

		void ClearShaderResource(unsigned int slot);

		void LogDebugInfo();

		inline HWND GetWindowHandle() { return hwnd; }
		inline UINT GetWidth()        { return backBufferDesc.Width; }
		inline UINT GetHeight()       { return backBufferDesc.Height; }

		bool ScreenToClientClamped(POINT& point); // returns true if the point was clamped to the client area
		void NormaliseCoordinates(POINT& point, float& x, float& y); // convert to NDC

		// helper function to update a buffer
		template <typename T>
		static void UpdateBuffer(ID3D11Buffer* pBuffer, std::vector<T>& source) {
			D3D11_MAPPED_SUBRESOURCE mappedSubResource;
			GetContext().Map(pBuffer, 0u, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubResource);
			memcpy(mappedSubResource.pData, source.data(), sizeof(T) * source.size());
			GetContext().Unmap(pBuffer, 0);
		}

		// helper function to update a buffer, limiting the count
		template <typename T>
		static void UpdateBuffer(ID3D11Buffer* pBuffer, std::vector<T>& source, unsigned int count) {
			E2_ASSERT(count <= source.size(), "Attempting to update count greater than source size");
			D3D11_MAPPED_SUBRESOURCE mappedSubResource;
			GetContext().Map(pBuffer, 0u, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubResource);
			memcpy(mappedSubResource.pData, source.data(), sizeof(T) * count);
			GetContext().Unmap(pBuffer, 0);
		}

	protected:
		static std::unique_ptr<DXDevice> instance;

		HWND hwnd;

		// core device resources
		wrl::ComPtr<IDXGISwapChain1>      pSwapChain = nullptr;
		wrl::ComPtr<ID3D11Device3>        pDevice = nullptr;
		wrl::ComPtr<ID3D11DeviceContext3> pImmediateContext = nullptr;

		// back buffer
		wrl::ComPtr<ID3D11Texture2D>        pBackBuffer = nullptr;
		wrl::ComPtr<ID3D11RenderTargetView> pTargetView = nullptr;
		D3D11_TEXTURE2D_DESC                backBufferDesc;

		// depth buffer
		wrl::ComPtr<ID3D11DepthStencilView>  pDepthStencilView = nullptr;
		wrl::ComPtr<ID3D11Texture2D>         pDepthTexture = nullptr;
		wrl::ComPtr<ID3D11DepthStencilState> pDepthStencilState = nullptr;

		void CreateDeviceAndSwapchain();
		void ConfigurePipeline();
		void ReleasePipeline();

		// states for resue
		wrl::ComPtr<ID3D11RasterizerState> pRSDefault;
		wrl::ComPtr<ID3D11RasterizerState> pRSWireframe;
		wrl::ComPtr<ID3D11RasterizerState> pRSNoFaceCulling;
		wrl::ComPtr<ID3D11BlendState>      pAlphaBlendState;
	};

}