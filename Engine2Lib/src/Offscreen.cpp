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
		Configure();
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
		if (pDepthStencilView) // has depth buffer
		{
			DXDevice::GetContext().OMSetRenderTargets(1u, pTargetView.GetAddressOf(), pDepthStencilView.Get());
			DXDevice::GetContext().OMSetDepthStencilState(pDepthStencilState.Get(), stencilRef);
		}
		else // no depth buffer
		{
			DXDevice::GetContext().OMSetRenderTargets(1u, pTargetView.GetAddressOf(), nullptr);
		}
	}

	void Offscreen::SetAsTargetNoDepthBuffer()
	{
		DXDevice::GetContext().OMSetRenderTargets(1u, pTargetView.GetAddressOf(), nullptr);
	}

	void Offscreen::RemoveAsTarget()
	{
		DXDevice::Get().SetBackBufferAsRenderTarget();
	}

	void Offscreen::Draw()
	{
		Bind(); // bind this as resource
		pVS->Bind();
		pPS->Bind();
		pDrawable->Bind();
		pDrawable->Draw();
		Unbind();
	}

	void Offscreen::DrawToBackBuffer()
	{
		DXDevice::Get().SetBackBufferAsRenderTargetNoDepthCheck();
		Draw();
		DXDevice::Get().SetBackBufferAsRenderTarget();
	}

	void Offscreen::Clear()
	{
		constexpr float clearColor[4] = { 0.0f,0.0f,0.0f,1.0f };
		DXDevice::GetContext().ClearRenderTargetView(pTargetView.Get(), clearColor);

		if (pDepthStencilView)
		{
			DXDevice::GetContext().ClearDepthStencilView(pDepthStencilView.Get(), D3D11_CLEAR_FLAG::D3D11_CLEAR_DEPTH, 1.0f, 0u);
		}
	}

	void Offscreen::Release()
	{
		pBuffer.Reset();
		pTargetView.Reset();

		pDepthStencilView.Reset();
		pDepthTexture.Reset();
		pDepthStencilState.Reset();

		pResourceView.Reset();
		pSamplerState.Reset();
	}

	void Offscreen::OnImgui()
	{
		if (ImGui::CollapsingHeader("Offscreen target"))
		{
			ImGui::Text("Slot %i", slot);

			if (pDepthStencilView) ImGui::Text("Has depth buffer");
			else ImGui::Text("No depth buffer");
		}
	}

	void Offscreen::InitialiseDrawResources()
	{
		struct Vertex {
			float position[3];
			float texCoord[2];
		};

		VertexLayoutSimple::VertexShaderLayout vsLayout = {
			{"Position", DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT},
			{"TexCoord", DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT},
		};
		auto layout = VertexLayoutSimple::ToDescriptor(vsLayout);

		std::vector<Vertex> verticies = {
			{ {-1.0f, -1.0f, 0.0f}, {0.0f, 1.0f} },
			{ {-1.0f,  1.0f, 0.0f}, {0.0f, 0.0f} },
			{ { 1.0f,  1.0f, 0.0f}, {1.0f, 0.0f} },
			{ {-1.0f, -1.0f, 0.0f}, {0.0f, 1.0f} },
			{ { 1.0f,  1.0f, 0.0f}, {1.0f, 0.0f} },
			{ { 1.0f, -1.0f, 0.0f}, {1.0f, 1.0f} },
		};


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
		pDrawable = std::make_shared<MeshTriangleList<Vertex>>(verticies);
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

		width = bufferTexureDesc.Width;
		height = bufferTexureDesc.Height;
		format = bufferTexureDesc.Format;
	}


	void Offscreen::InitialiseDepthBuffer()
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
		dtDesc.Format = DXGI_FORMAT::DXGI_FORMAT_D32_FLOAT;
		dtDesc.SampleDesc.Count = 1u;
		dtDesc.SampleDesc.Quality = 0u;
		dtDesc.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
		dtDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_DEPTH_STENCIL;
		dtDesc.CPUAccessFlags = 0u;
		dtDesc.MiscFlags = 0u;

		hr = DXDevice::GetDevice().CreateTexture2D(&dtDesc, nullptr, &pDepthTexture);

		E2_ASSERT_HR(hr, "CreateTexture2D failed for DepthStencil");

		D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
		dsvDesc.Format = dtDesc.Format;
		dsvDesc.ViewDimension = D3D11_DSV_DIMENSION::D3D11_DSV_DIMENSION_TEXTURE2D;
		dsvDesc.Flags = 0;
		dsvDesc.Texture2D.MipSlice = 0u;

		hr = DXDevice::GetDevice().CreateDepthStencilView(pDepthTexture.Get(), &dsvDesc, &pDepthStencilView);

		E2_ASSERT_HR(hr, "CreateDepthStencilView failed for DepthStencil");
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

}