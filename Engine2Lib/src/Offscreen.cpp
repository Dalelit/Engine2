#pragma once
#include "pch.h"
#include "Offscreen.h"
#include "DXDevice.h"
#include "VertexBuffer.h"

namespace Engine2
{
	Offscreen::Offscreen(unsigned int slot) : slot(slot)
	{
		InitialiseDrawResources();
		InitialiseSubDisplayVB();
		Configure();

		// sub display uses these simple version that may get overridden
		subDisplay.pVS = pVS;
		subDisplay.pPS = pPS;
	}

	void Offscreen::Bind()
	{
		DXDevice::GetContext().PSSetShaderResources(slot, 1u, pResourceView.GetAddressOf());
		DXDevice::GetContext().PSSetSamplers(slot, 1u, pSamplerState.GetAddressOf());
	}

	void Offscreen::Unbind()
	{
		DXDevice::Get().ClearShaderResource(slot);
	}

	void Offscreen::SetAsTarget()
	{
		DXDevice::GetContext().OMSetRenderTargets(1u, pTargetView.GetAddressOf(), nullptr);
	}

	void Offscreen::DrawToBackBuffer()
	{
		DXDevice::Get().SetBackBufferAsRenderTargetNoDepthCheck();
		Bind(); // bind this as resource
		pVS->Bind();
		pPS->Bind();
		pDrawable->BindAndDraw();
		Unbind();
		DXDevice::Get().SetBackBufferAsRenderTarget();
	}

	void Offscreen::Clear()
	{
		constexpr float clearColor[4] = { 0.0f,0.0f,0.0f,0.0f };
		DXDevice::GetContext().ClearRenderTargetView(pTargetView.Get(), clearColor);
	}

	void Offscreen::Release()
	{
		pBuffer.Reset();
		pTargetView.Reset();

		pResourceView.Reset();
		pSamplerState.Reset();
	}

	void Offscreen::OnImgui(bool subNode)
	{
		if (subNode || ImGui::TreeNode("Offscreen target")) // skip the tree node if a subnode (i.e. depth buffer version called this)
		{
			ImGui::Text("Slot %i", slot);
			if (ImGui::TreeNode("Render target"))
			{
				ImGui::Checkbox("Show", &subDisplay.show);
				if (ImGui::DragFloat2("Left,Top", subDisplay.leftTop, 0.05f, -1.0f, 1.0f)) InitialiseSubDisplayVB();
				if (ImGui::DragFloat("Size", &subDisplay.size, 0.05f, 0.1f)) InitialiseSubDisplayVB();
				ImGui::TreePop();
			}

			if (!subNode) ImGui::TreePop();
		}
	}

	void Offscreen::InitialiseDrawResources()
	{
		VertexLayoutSimple::VertexShaderLayout vsLayout = {
			{"Position", DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT},
			{"TexCoord", DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT},
		};
		auto layout = VertexLayoutSimple::ToDescriptor(vsLayout);

		std::string VSsrc = R"(
			struct VSOut
			{
				float2 texCoord : TexCoord;
				float4 pos : SV_POSITION;
			};

			VSOut main(float3 pos : Position, float2 texCoord : TexCoord)
			{
				VSOut vso;

				vso.pos = float4(pos, 1.0f);
				vso.texCoord = texCoord;

				return vso;
			}
		)";

		std::string PSsrc = R"(
			Texture2D tex;
			SamplerState smplr;

			float4 main(float2 texCoord : TexCoord) : SV_TARGET
			{
				return tex.Sample(smplr, texCoord);
			}
		)";

		pVS = VertexShader::CreateFromString(VSsrc, layout);
		pPS = PixelShader::CreateFromString(PSsrc);
		pDrawable = CreateVertexBuffer(-1.0f, 1.0f, 1.0f, -1.0f);
	}

	void Offscreen::InitialiseBuffer()
	{
		HRESULT hr;
		
		D3D11_TEXTURE2D_DESC bufferTexureDesc = DXDevice::Get().GetBackBufferTextureDesc();  // default to the back buffer desc
		bufferTexureDesc.BindFlags = bufferTexureDesc.BindFlags | D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE;

		hr = DXDevice::GetDevice().CreateTexture2D(&bufferTexureDesc, nullptr, &pBuffer);

		E2_ASSERT_HR(hr, "CreateTexture2D for offscreen failed");

		hr = DXDevice::GetDevice().CreateRenderTargetView(pBuffer.Get(), nullptr, &pTargetView);

		E2_ASSERT_HR(hr, "CreateRenderTargetView for offscreen failed");

		width  = bufferTexureDesc.Width;
		height = bufferTexureDesc.Height;
		format = bufferTexureDesc.Format;
	}

	void Offscreen::InitialiseShaderResources()
	{
		HRESULT hr;

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION::D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = 1;

		hr = DXDevice::GetDevice().CreateShaderResourceView(pBuffer.Get(), &srvDesc, &pResourceView);

		E2_ASSERT_HR(hr, "CreateShaderResourceView failed");

		D3D11_SAMPLER_DESC sampDesc = {};
		sampDesc.Filter = D3D11_FILTER::D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_CLAMP;
		sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_CLAMP;
		sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_CLAMP;

		hr = DXDevice::GetDevice().CreateSamplerState(&sampDesc, &pSamplerState);

		E2_ASSERT_HR(hr, "CreateSamplerState failed");
	}

	std::shared_ptr<Drawable> Offscreen::CreateVertexBuffer(float left, float top, float right, float bottom)
	{
		struct Vertex {
			float position[3];
			float texCoord[2];
		};

		std::vector<Vertex> verticies = {
			{ { left, bottom, 0.0f}, {0.0f, 1.0f} },
			{ { left,    top, 0.0f}, {0.0f, 0.0f} },
			{ {right,    top, 0.0f}, {1.0f, 0.0f} },
			{ { left, bottom, 0.0f}, {0.0f, 1.0f} },
			{ {right,    top, 0.0f}, {1.0f, 0.0f} },
			{ {right, bottom, 0.0f}, {1.0f, 1.0f} },
		};

		return std::make_shared<MeshTriangleList<Vertex>>(verticies);
	}

	void Offscreen::InitialiseSubDisplayVB()
	{
		float left = subDisplay.leftTop[0];
		float top = subDisplay.leftTop[1];
		float right = subDisplay.leftTop[0] + subDisplay.size;
		float bottom = subDisplay.leftTop[1] - subDisplay.size;

		subDisplay.pVB = CreateVertexBuffer(left, top, right, bottom);
	}

	void Offscreen::ShowSubDisplayRenderTarget()
	{
		if (subDisplay.show)
		{
			DXDevice::Get().SetBackBufferAsRenderTargetNoDepthCheck();
			Bind();
			subDisplay.pVS->Bind();
			subDisplay.pPS->Bind();
			subDisplay.pVB->BindAndDraw();
			DXDevice::Get().ClearShaderResource(slot); // Unbind();
			DXDevice::Get().SetBackBufferAsRenderTarget();
		}
	}

	OffscreenWithDepthBuffer::OffscreenWithDepthBuffer(unsigned int slot) : Offscreen(slot)
	{
		InitialiseDepthBuffer();
		InitialiseSubDisplayDepthBufferVB();
		subDisplayDepthBuffer.pVS = pVS;

		// pixel shader is specific for depth buffer
		std::string PSsrc = R"(
			Texture2D tex;
			SamplerState smplr;

			float4 main(float2 texCoord : TexCoord) : SV_TARGET
			{
				float depth = 1.0 - tex.Sample(smplr, texCoord);
				return float4(depth, depth, depth, 1.0);
			}
		)";

		subDisplayDepthBuffer.pPS = PixelShader::CreateFromString(PSsrc);
	}

	void OffscreenWithDepthBuffer::SetAsTarget()
	{
		DXDevice::GetContext().OMSetRenderTargets(1u, pTargetView.GetAddressOf(), pDepthStencilView.Get());
		DXDevice::GetContext().OMSetDepthStencilState(pDepthStencilState.Get(), stencilRef);
	}

	void OffscreenWithDepthBuffer::Clear()
	{
		Offscreen::Clear();
		DXDevice::GetContext().ClearDepthStencilView(pDepthStencilView.Get(), D3D11_CLEAR_FLAG::D3D11_CLEAR_DEPTH, 1.0f, 0u);
	}

	void OffscreenWithDepthBuffer::Release()
	{
		Offscreen::Release();
		pDepthStencilView.Reset();
		pDepthTexture.Reset();
		pDepthStencilState.Reset();
	}

	void OffscreenWithDepthBuffer::OnImgui()
	{
		if (ImGui::TreeNode("Offscreen target"))
		{
			Offscreen::OnImgui(true);
			if (ImGui::TreeNode("Depth buffer"))
			{
				ImGui::Checkbox("Show", &subDisplayDepthBuffer.show);
				if (ImGui::DragFloat2("Left,Top", subDisplayDepthBuffer.leftTop, 0.05f, -1.0f, 1.0f)) InitialiseSubDisplayDepthBufferVB();
				if (ImGui::DragFloat("Size", &subDisplayDepthBuffer.size, 0.05f, 0.1f)) InitialiseSubDisplayDepthBufferVB();
				ImGui::TreePop();
			}
			ImGui::TreePop();
		}
	}

	void OffscreenWithDepthBuffer::ShowSubDisplayDepthBuffer()
	{
		if (subDisplayDepthBuffer.show)
		{
			DXDevice::Get().SetBackBufferAsRenderTargetNoDepthCheck();

			// bind depth buffer as resource
			DXDevice::GetContext().PSSetShaderResources(slot, 1u, subDisplayDepthBuffer.pDTRV.GetAddressOf());
			DXDevice::GetContext().PSSetSamplers(slot, 1u, pSamplerState.GetAddressOf()); // reuse

			subDisplayDepthBuffer.pVS->Bind();
			subDisplayDepthBuffer.pPS->Bind();
			subDisplayDepthBuffer.pVB->BindAndDraw();
			DXDevice::Get().ClearShaderResource(slot); // Unbind();
			DXDevice::Get().SetBackBufferAsRenderTarget();
		}
	}

	void OffscreenWithDepthBuffer::InitialiseDepthBuffer()
	{
		HRESULT hr;

		D3D11_DEPTH_STENCIL_DESC depthStencilDesc = CD3D11_DEPTH_STENCIL_DESC(CD3D11_DEFAULT());

		hr = DXDevice::GetDevice().CreateDepthStencilState(&depthStencilDesc, &pDepthStencilState);

		E2_ASSERT_HR(hr, "CreateDepthStencilState failed for DepthStencil");

		D3D11_TEXTURE2D_DESC dtDesc = {};
		dtDesc.Width = width;
		dtDesc.Height = height;
		dtDesc.MipLevels = 1u;
		dtDesc.ArraySize = 1u;
		dtDesc.Format = DXGI_FORMAT::DXGI_FORMAT_R32_TYPELESS; // DXGI_FORMAT::DXGI_FORMAT_D32_FLOAT - cannot use this and bind as a shader resource
		dtDesc.SampleDesc.Count = 1u;
		dtDesc.SampleDesc.Quality = 0u;
		dtDesc.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
		dtDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE; // so we can also draw it
		dtDesc.CPUAccessFlags = 0u;
		dtDesc.MiscFlags = 0u;

		hr = DXDevice::GetDevice().CreateTexture2D(&dtDesc, nullptr, &pDepthTexture);

		E2_ASSERT_HR(hr, "CreateTexture2D failed for DepthStencil");

		D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
		dsvDesc.Format = DXGI_FORMAT::DXGI_FORMAT_D32_FLOAT; // dtDesc.Format - use the correct format rather than typeless
		dsvDesc.ViewDimension = D3D11_DSV_DIMENSION::D3D11_DSV_DIMENSION_TEXTURE2D;
		dsvDesc.Flags = 0;
		dsvDesc.Texture2D.MipSlice = 0u;

		hr = DXDevice::GetDevice().CreateDepthStencilView(pDepthTexture.Get(), &dsvDesc, &pDepthStencilView);

		E2_ASSERT_HR(hr, "CreateDepthStencilView failed for DepthStencil");

		// shader resource view so the depth buffer can be displayed
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = DXGI_FORMAT::DXGI_FORMAT_R32_FLOAT;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION::D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = 1;

		hr = DXDevice::GetDevice().CreateShaderResourceView(pDepthTexture.Get(), &srvDesc, &subDisplayDepthBuffer.pDTRV);

		E2_ASSERT_HR(hr, "CreateShaderResourceView failed");
	}

	void OffscreenWithDepthBuffer::InitialiseSubDisplayDepthBufferVB()
	{
		float left = subDisplayDepthBuffer.leftTop[0];
		float top = subDisplayDepthBuffer.leftTop[1];
		float right = subDisplayDepthBuffer.leftTop[0] + subDisplayDepthBuffer.size;
		float bottom = subDisplayDepthBuffer.leftTop[1] - subDisplayDepthBuffer.size;

		subDisplayDepthBuffer.pVB = CreateVertexBuffer(left, top, right, bottom);
	}

}