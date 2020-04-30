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
		float ratioToFullscreen;
		UINT stencilRef = 0;
		UINT width;
		UINT height;

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
		static DXDevice& CreateDevice(HWND hwnd);
		inline static DXDevice& Get() { return *instance; }
		inline static ID3D11Device3& GetDevice() { return *instance->pDevice.Get(); }
		inline static ID3D11DeviceContext3& GetContext() { return *instance->pImmediateContext.Get(); }

		DXDevice(HWND hwnd);
		~DXDevice() = default;

		void BeginFrame();
		void PresentFrame();

		void ScreenSizeChanged();

		float GetAspectRatio() { return (float)backBufferDesc.Width / (float)backBufferDesc.Height; }

		unsigned int CreateOffscreenRenderTarget(bool withDepthBuffer = true, float ratioToFullscreen = 1.0f);
		inline RenderTarget& GetRenderTarget(unsigned int id) { return renderTargets[id]; };

		void BindRenderTargetAsTarget(unsigned int id);
		void BindRenderTargetAsResource(unsigned int id, unsigned int slot);
		void UnbindRenderTargetAsResource(unsigned int id, unsigned int slot);

		unsigned int GetBackbufferRenderTargetId() { return 0; } // do this way in case the number should change for some reason
		void BindBackbufferRenderTarget() { BindRenderTargetAsTarget(0); }
		inline RenderTarget& GetBackbufferRenderTargetResource() { return renderTargets[GetBackbufferRenderTargetId()]; }
		void BindBackbufferNoDepthbuffer(); // to do: hack to remove

		void SetWireframeRenderState();
		void SetDefaultRenderState();

		void LogDebugInfo();

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

		// render states for resue
		wrl::ComPtr<ID3D11RasterizerState> pRSDefault;
		wrl::ComPtr<ID3D11RasterizerState> pRSWireframe;
	};

}