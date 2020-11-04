#include "pch.h"
#include "OffscreenOutliner.h"

namespace Engine2
{
	OffscreenOutliner::OffscreenOutliner()
	{
		vsOutlineCB.slot = 2;
		Initialise();
		Configure();
	}

	void OffscreenOutliner::Clear()
	{
		offscreen.Clear();
		DXDevice::GetContext().ClearDepthStencilView(pDSVStencil.Get(), D3D11_CLEAR_FLAG::D3D11_CLEAR_DEPTH | D3D11_CLEAR_FLAG::D3D11_CLEAR_STENCIL, 1.0f, 0u);
	}

	void OffscreenOutliner::DrawToBackBuffer()
	{
		DXDevice::Get().SetAlphaBlendState();
		offscreen.DrawToBackBuffer();
		DXDevice::Get().SetDefaultBlendState();
	}
	
	void OffscreenOutliner::SetForWriteMask()
	{
		pVSOutline->Bind(); // used for both
		vsOutlineCB.data.m128_f32[0] = 1.0f;
		vsOutlineCB.Bind();
		DXDevice::GetContext().PSSetShader(nullptr, nullptr, 0);
		DXDevice::GetContext().OMSetRenderTargets(0u, nullptr, pDSVStencil.Get());
		DXDevice::GetContext().OMSetDepthStencilState(pDSSWriteMask.Get(), 0xFF);
	}
	
	void OffscreenOutliner::SetForOutline()
	{
		// pVSOutline->Bind(); called in SetForMask
		vsOutlineCB.data.m128_f32[0] = outlineScale;
		vsOutlineCB.Bind();
		psOutlineCB.data = outlineColor;
		psOutlineCB.Bind();
		pPSOutline->Bind();
		DXDevice::GetContext().OMSetRenderTargets(1u, offscreen.GetRenderTargetView().GetAddressOf(), pDSVStencil.Get());
		DXDevice::GetContext().OMSetDepthStencilState(pDSSUseMask.Get(), 0xFF);
	}

	void OffscreenOutliner::OnImgui()
	{
		ImGui::DragFloat("Outline scale", &outlineScale, 0.05f, 1.0f, 2.0f);
		ImGui::ColorEdit4("Outline color", outlineColor.m128_f32);
	}

	void OffscreenOutliner::Initialise()
	{
		// shaders for outline
		std::vector<D3D11_INPUT_ELEMENT_DESC> vsLayout = {
			{"Position",    0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
		};

		std::string vsFileName = Config::directories["ShaderCompiledDir"] + "OutlineVS.cso";
		pVSOutline = VertexShader::CreateFromCompiledFile(vsFileName, vsLayout);

		std::string psFileName = Config::directories["ShaderCompiledDir"] + "OutlinePS.cso";
		pPSOutline = PixelShader::CreateFromCompiledFile(psFileName);
	}
	
	void OffscreenOutliner::Configure()
	{
		HRESULT hr;

		D3D11_DEPTH_STENCIL_DESC stencilWriteDesc = CD3D11_DEPTH_STENCIL_DESC(CD3D11_DEFAULT());
		D3D11_DEPTH_STENCIL_DESC stencilMaskDesc = CD3D11_DEPTH_STENCIL_DESC(CD3D11_DEFAULT());
		D3D11_DEPTH_STENCILOP_DESC readOps;
		readOps.StencilFailOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
		readOps.StencilDepthFailOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
		// note: read/write masks set to 0xFF

		// Write
		readOps.StencilPassOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_REPLACE;
		readOps.StencilFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_ALWAYS;
		stencilWriteDesc.DepthEnable = false;
		stencilWriteDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ZERO;
		stencilWriteDesc.StencilEnable = true;
		stencilWriteDesc.StencilWriteMask = 0xFF;
		stencilWriteDesc.FrontFace = readOps;
		stencilWriteDesc.BackFace = readOps;
		hr = DXDevice::GetDevice().CreateDepthStencilState(&stencilWriteDesc, &pDSSWriteMask);
		E2_ASSERT_HR(hr, "CreateDepthStencilState for stencil write failed");

		// Mask
		readOps.StencilPassOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
		readOps.StencilFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_NOT_EQUAL;
		stencilMaskDesc.DepthEnable = false;
		stencilMaskDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ZERO;
		stencilMaskDesc.StencilEnable = true;
		stencilMaskDesc.StencilReadMask = 0xFF;
		stencilMaskDesc.FrontFace = readOps;
		stencilMaskDesc.BackFace = readOps;

		hr = DXDevice::GetDevice().CreateDepthStencilState(&stencilMaskDesc, &pDSSUseMask);
		E2_ASSERT_HR(hr, "CreateDepthStencilState for stencil Mask failed");

		// Depth stencil

		D3D11_TEXTURE2D_DESC dtDesc = {};
		dtDesc.Width = offscreen.GetWidth();
		dtDesc.Height = offscreen.GetHeight();
		dtDesc.MipLevels = 1u;
		dtDesc.ArraySize = 1u;
		dtDesc.Format = DXGI_FORMAT::DXGI_FORMAT_D24_UNORM_S8_UINT;
		dtDesc.SampleDesc.Count = 1u;
		dtDesc.SampleDesc.Quality = 0u;
		dtDesc.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
		dtDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_DEPTH_STENCIL;
		dtDesc.CPUAccessFlags = 0u;
		dtDesc.MiscFlags = 0u;

		hr = DXDevice::GetDevice().CreateTexture2D(&dtDesc, nullptr, &pDTStencil);

		E2_ASSERT_HR(hr, "CreateTexture2D failed");

		D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
		dsvDesc.Format = dtDesc.Format;
		dsvDesc.ViewDimension = D3D11_DSV_DIMENSION::D3D11_DSV_DIMENSION_TEXTURE2D;
		dsvDesc.Flags = 0;
		dsvDesc.Texture2D.MipSlice = 0u;

		hr = DXDevice::GetDevice().CreateDepthStencilView(pDTStencil.Get(), &dsvDesc, &pDSVStencil);

		E2_ASSERT_HR(hr, "CreateDepthStencilView failed");
	}

	void OffscreenOutliner::Reset()
	{
		pDSSWriteMask.Reset();
		pDSSUseMask.Reset();
		pDTStencil.Reset();
		pDSVStencil.Reset();
	}
}