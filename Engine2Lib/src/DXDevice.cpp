#include "pch.h"
#include "Common.h"
#include "DXDevice.h"

#pragma comment(lib,"d3d11.lib")
#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib,"dxguid.lib")

#define E2DX_DEBUG_LOG(msg)       OutputDebugStringA(msg);

namespace Engine2
{
	std::unique_ptr<DXDevice> DXDevice::instance = nullptr;

	DXDevice& DXDevice::CreateDevice(HWND hwnd)
	{
		E2_ASSERT(instance == nullptr, "DXDevice instance already exists");

		instance = std::make_unique<DXDevice>(hwnd);

		return *instance.get();
	}

	DXDevice::DXDevice(HWND hwnd) :
		hwnd(hwnd)
	{
		renderTargets.emplace_back(); // create the placeholder back buffer target at index 0
		CreateDeviceAndSwapchain();
		ConfigurePipeline();

		// create the render states for future use
		pRSDefault = nullptr;

		D3D11_RASTERIZER_DESC rsDesc = CD3D11_RASTERIZER_DESC(CD3D11_DEFAULT());
		rsDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_WIREFRAME;
		pDevice->CreateRasterizerState(&rsDesc, &pRSWireframe);
	}

	void DXDevice::BeginFrame()
	{
		for (auto& rt : renderTargets)
		{
			pImmediateContext->ClearRenderTargetView(rt.pTargetView.Get(), clearColor);
			if (rt.depthBuffer)
				pImmediateContext->ClearDepthStencilView(rt.pDepthStencilView.Get(), D3D11_CLEAR_FLAG::D3D11_CLEAR_DEPTH, 1.0f, 0u);
		}

		BindBackbufferRenderTarget();
	}

	void DXDevice::PresentFrame()
	{
		pSwapChain->Present(1u, 0u);
	}

	void DXDevice::ScreenSizeChanged()
	{
		// release the attached pipeline
		ReleasePipeline();

		HRESULT hr = pSwapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT::DXGI_FORMAT_UNKNOWN, 0); // resize, retaining previous settings
		E2_ASSERT_HR(hr, "ResizeBuffers failed");

		// recreate the pipeline
		ConfigurePipeline();
	}

	unsigned int DXDevice::CreateOffscreenRenderTarget(bool withDepthBuffer)
	{
		unsigned int indx = (unsigned int)renderTargets.size();
		renderTargets.emplace_back();
		renderTargets[indx].depthBuffer = withDepthBuffer;
		ConfigureRenderTarget(indx);
		return indx;
	}

	void DXDevice::BindRenderTargetAsTarget(unsigned int id)
	{
		RenderTarget& rt = renderTargets[id];

		if (rt.depthBuffer)
		{
			pImmediateContext->OMSetRenderTargets(1u, rt.pTargetView.GetAddressOf(), rt.pDepthStencilView.Get());
			pImmediateContext->OMSetDepthStencilState(rt.pDepthStencilState.Get(), 0u);
		}
		else
		{
			pImmediateContext->OMSetRenderTargets(1u, rt.pTargetView.GetAddressOf(), nullptr);
		}
	}

	void DXDevice::BindRenderTargetAsResource(unsigned int id, unsigned int slot)
	{
		E2_ASSERT(id != 0, "Assuming the back buffer at index 0 is never used as a resource");

		pImmediateContext->PSSetShaderResources(slot, 1u, renderTargets[id].pResourceView.GetAddressOf());
		pImmediateContext->PSSetSamplers(slot, 1u, renderTargets[id].pSamplerState.GetAddressOf());
	}

	void DXDevice::UnbindRenderTargetAsResource(unsigned int id, unsigned int slot)
	{
		E2_ASSERT(id != 0, "Assuming the back buffer at index 0 is never used as a resource");

		ID3D11ShaderResourceView* const srv[1] = { nullptr };
		pImmediateContext->PSSetShaderResources(slot, 1u, srv);
		pImmediateContext->PSSetSamplers(slot, 0u, nullptr);
	}

	void DXDevice::SetWireframeRenderState()
	{
		pImmediateContext->RSSetState(pRSWireframe.Get());
	}

	void DXDevice::SetDefaultRenderState()
	{
		pImmediateContext->RSSetState(pRSDefault.Get());
	}

	void DXDevice::LogDebugInfo()
	{
		HRESULT hr;
		std::stringstream msg;

		msg << "---- DX Device debug Info ----" << std::endl;

		msg << "D3D11_TEXTURE2D_DESC backBufferDesc" << std::endl;
		msg << " Format " << backBufferDesc.Format << std::endl;
		msg << " Width " << backBufferDesc.Width << std::endl;
		msg << " Height " << backBufferDesc.Height << std::endl;

		wrl::ComPtr<IDXGIDevice3> pDxgiDevice;
		pDevice.As(&pDxgiDevice);

		wrl::ComPtr<IDXGIAdapter> pAdapter;
		pDxgiDevice->GetAdapter(&pAdapter);

		DXGI_ADAPTER_DESC desc;
		pAdapter->GetDesc(&desc);

		msg << "DXGI_ADAPTER_DESC" << std::endl;
		msg << " Description " << desc.Description << std::endl;
		msg << " Revision " << desc.Revision << std::endl;

		wrl::ComPtr<ID3D11Debug> pDebug = nullptr;
		hr = pDevice->QueryInterface(__uuidof(ID3D11Debug), &pDebug);
		E2_ASSERT_HR(hr, "Failed to get ID3D11Debug");

		msg << "------------------------------" << std::endl;
		E2DX_DEBUG_LOG(msg.str().c_str());

		pDebug->ReportLiveDeviceObjects(D3D11_RLDO_SUMMARY);
		//pDebug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
	}

	void DXDevice::CreateDeviceAndSwapchain()
	{
		HRESULT hr;

		// for the create call, then get stored as later versions
		wrl::ComPtr<IDXGISwapChain> pSwapChain0 = nullptr;
		wrl::ComPtr<ID3D11Device> pDevice0 = nullptr;
		wrl::ComPtr<ID3D11DeviceContext> pImmediateContext0 = nullptr;

		/////////////////// create device and back buffer

		DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
		swapChainDesc.BufferDesc.Width = 0;
		swapChainDesc.BufferDesc.Height = 0;
		swapChainDesc.BufferDesc.Format = DXGI_FORMAT::DXGI_FORMAT_B8G8R8A8_UNORM;
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 0;
		swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING::DXGI_MODE_SCALING_UNSPECIFIED;
		swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER::DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = 2u;
		swapChainDesc.OutputWindow = hwnd;
		swapChainDesc.Windowed = TRUE;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT::DXGI_SWAP_EFFECT_FLIP_DISCARD; // DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL?
		swapChainDesc.Flags = 0;

		D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_11_1 };
		D3D_FEATURE_LEVEL featureLevel; // will get populated with the actual feature level used... wanting 11_1

		hr = D3D11CreateDeviceAndSwapChain(
			nullptr,
			D3D_DRIVER_TYPE::D3D_DRIVER_TYPE_HARDWARE,
			NULL,
			D3D11_CREATE_DEVICE_FLAG::D3D11_CREATE_DEVICE_DEBUG | D3D11_CREATE_DEVICE_FLAG::D3D11_CREATE_DEVICE_BGRA_SUPPORT,
			featureLevels,
			ARRAYSIZE(featureLevels),
			D3D11_SDK_VERSION,
			&swapChainDesc,
			&pSwapChain0,
			&pDevice0,
			&featureLevel,
			&pImmediateContext0
		);
		E2_ASSERT_HR(hr, "D3D11CreateDeviceAndSwapChain failed");

		// store them as later versions of the interface
		hr = pSwapChain0.As(&pSwapChain);               E2_ASSERT_HR(hr, "Conversion of swapchain version failed");
		hr = pDevice0.As(&pDevice);                     E2_ASSERT_HR(hr, "Conversion of device version failed");
		hr = pImmediateContext0.As(&pImmediateContext); E2_ASSERT_HR(hr, "Conversion of context version failed");
	}

	void DXDevice::SetBackBuffer()
	{
		HRESULT hr;
		RenderTarget& rt = renderTargets[0];

		hr = pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), &rt.pBuffer);

		E2_ASSERT_HR(hr, "GetBuffer failed");

		hr = pDevice->CreateRenderTargetView(rt.pBuffer.Get(), nullptr, &rt.pTargetView);

		E2_ASSERT_HR(hr, "CreateRenderTargetView failed");

		rt.pBuffer->GetDesc(&backBufferDesc);

		D3D11_VIEWPORT viewport = {};
		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;
		viewport.Width = (float)backBufferDesc.Width;
		viewport.Height = (float)backBufferDesc.Height;
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;

		pImmediateContext->RSSetViewports(1, &viewport);

		ConfigureDepthBuffer(0); // and the depth buffer for it
	}

	void DXDevice::ConfigureDepthBuffer(unsigned int indx)
	{
		HRESULT hr;
		RenderTarget& rt = renderTargets[indx];

		D3D11_DEPTH_STENCIL_DESC dsDesc = {};
		dsDesc.DepthEnable = TRUE;
		dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ALL;
		dsDesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS;
		dsDesc.StencilEnable = FALSE;

		hr = pDevice->CreateDepthStencilState(&dsDesc, &rt.pDepthStencilState);

		E2_ASSERT_HR(hr, "CreateDepthStencilState failed");

		D3D11_TEXTURE2D_DESC dtDesc = {};
		dtDesc.Width = backBufferDesc.Width;
		dtDesc.Height = backBufferDesc.Height;
		dtDesc.MipLevels = 1u;
		dtDesc.ArraySize = 1u;
		dtDesc.Format = DXGI_FORMAT::DXGI_FORMAT_D32_FLOAT;
		dtDesc.SampleDesc.Count = 1u;
		dtDesc.SampleDesc.Quality = 0u;
		dtDesc.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
		dtDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_DEPTH_STENCIL;
		dtDesc.CPUAccessFlags = 0u;
		dtDesc.MiscFlags = 0u;

		hr = pDevice->CreateTexture2D(&dtDesc, nullptr, &rt.pDepthTexture);

		E2_ASSERT_HR(hr, "CreateTexture2D failed");

		D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
		dsvDesc.Format = dtDesc.Format;
		dsvDesc.ViewDimension = D3D11_DSV_DIMENSION::D3D11_DSV_DIMENSION_TEXTURE2D;
		dsvDesc.Flags = 0;
		dsvDesc.Texture2D.MipSlice = 0u;

		hr = pDevice->CreateDepthStencilView(rt.pDepthTexture.Get(), &dsvDesc, &rt.pDepthStencilView);

		E2_ASSERT_HR(hr, "CreateDepthStencilView failed");
	}

	void DXDevice::ConfigurePipeline()
	{
		SetBackBuffer(); // sort out the back buffer

		// reconfigure the offscreen targets.
		// note: 0 is the back buffer which is already in the vector.
		for (size_t i = 1; i < renderTargets.size(); i++) ConfigureRenderTarget((unsigned int)i);
	}

	void DXDevice::ReleasePipeline()
	{
		pImmediateContext->OMSetRenderTargets(0, 0, 0);

		for (auto& rt : renderTargets) rt.Reset();

		pImmediateContext->ClearState();
		pImmediateContext->Flush();
	}

	void DXDevice::ConfigureRenderTarget(unsigned int indx)
	{
		HRESULT hr;
		RenderTarget& rt = renderTargets[indx];

		D3D11_TEXTURE2D_DESC texDesc = backBufferDesc; // default to the back buffer desc
		texDesc.BindFlags = texDesc.BindFlags | D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE;

		hr = pDevice->CreateTexture2D(&texDesc, nullptr, &rt.pBuffer);

		E2_ASSERT_HR(hr, "CreateTexture2D for full offscreen failed");

		hr = pDevice->CreateRenderTargetView(rt.pBuffer.Get(), nullptr, &rt.pTargetView);

		E2_ASSERT_HR(hr, "CreateRenderTargetView for full offscreen failed");

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = texDesc.Format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION::D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = 1;

		hr = pDevice->CreateShaderResourceView(rt.pBuffer.Get(), &srvDesc, &rt.pResourceView);

		E2_ASSERT_HR(hr, "CreateShaderResourceView failed");

		D3D11_SAMPLER_DESC sampDesc = {};
		sampDesc.Filter = D3D11_FILTER::D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_CLAMP;
		sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_CLAMP;
		sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_CLAMP;

		hr = pDevice->CreateSamplerState(&sampDesc, &rt.pSamplerState);

		E2_ASSERT_HR(hr, "CreateSamplerState failed");

		if (rt.depthBuffer) ConfigureDepthBuffer(indx);
	}
}
