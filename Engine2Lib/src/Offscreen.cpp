#pragma once
#include "pch.h"
#include "Offscreen.h"
#include "DXDevice.h"


namespace Engine2
{
	Offscreen::OffscreenCommonResources Offscreen::Common;
	std::map<std::string, std::shared_ptr<PixelShader>> Offscreen::pixelShaders;

	void Offscreen::InitialiseCommon()
	{
		if (!Common.initialised)
		{
			AddSampleFilters();

			Common.ForDrawToBackBuffer.pVB = CreateVertexBuffer(-1.0f, 1.0f, 1.0f, -1.0f);

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

			Common.ForDrawToBackBuffer.pVS = VertexShader::CreateFromString(VSsrc, layout);

			// pixel shader specific for depth buffer
			std::string PSsrc = R"(
				Texture2D tex;
				SamplerState smplr;

				float4 main(float2 texCoord : TexCoord) : SV_TARGET
				{
					float depth = 0.01 / ( 1.01 - tex.Sample(smplr, texCoord) );
					return float4(depth, depth, depth, 1.0);
				}
			)";

			// pixel shader specific for depth buffer raw
			std::string PSRawsrc = R"(
				Texture2D tex;
				SamplerState smplr;

				float4 main(float2 texCoord : TexCoord) : SV_TARGET
				{
					float depth = tex.Sample(smplr, texCoord);
					return float4(depth, depth, depth, 1.0);
				}
			)";

			Common.ForDrawToSubDisplay.pVS = Common.ForDrawToBackBuffer.pVS; // same simple VS.
			Common.ForDrawToSubDisplay.pPSBuffer = pixelShaders["Copy"];
			Common.ForDrawToSubDisplay.pPSDepthBuffer = PixelShader::CreateFromString(PSsrc);
			Common.ForDrawToSubDisplay.pPSDepthBufferRaw = PixelShader::CreateFromString(PSRawsrc);

			Common.initialised = true;
		}
	}

	Offscreen::Offscreen(bool hasRenderTarget, bool hasDepthBuffer, unsigned int slot)
	{
		Descriptor desc = {};
		desc.hasRenderTarget = hasRenderTarget;
		desc.hasDepthBuffer = hasDepthBuffer;
		desc.slot = slot;

		Initialise(desc);
	}

	void Offscreen::Initialise(Descriptor desc)
	{
		descriptor = desc;

		if (descriptor.height <= 0 || descriptor.width <= 0)
		{
			auto bufferTexureDesc = DXDevice::Get().GetBackBufferTextureDesc();  // to do: will change this for more flexible dimensions
			width = bufferTexureDesc.Width;
			height = bufferTexureDesc.Height;
		}
		else
		{
			width = descriptor.width;
			height = descriptor.height;
		}

		InitialiseCommon();

		// default starting pixel shader is the copy
		pixelShaderName = "Copy";
		pPS = pixelShaders[pixelShaderName];

		InitialiseSubDisplayVB();
		InitialiseSubDisplayDepthBufferVB();

		Configure();
	}

	void Offscreen::BindBuffer(unsigned int textureSlot)
	{
		DXDevice::GetContext().PSSetShaderResources(textureSlot, 1u, pBufferResourceView.GetAddressOf());
		DXDevice::GetContext().PSSetSamplers(textureSlot, 1u, pBufferSamplerState.GetAddressOf());
	}

	void Offscreen::BindDepthBuffer(unsigned int textureSlot)
	{
		DXDevice::GetContext().PSSetShaderResources(textureSlot, 1u, pDepthBufferResourceView.GetAddressOf());
		DXDevice::GetContext().PSSetSamplers(textureSlot, 1u, pDepthBufferSamplerState.GetAddressOf());
	}

	void Offscreen::Unbind()
	{
		DXDevice::Get().ClearShaderResource(descriptor.slot);
	}

	void Offscreen::SetAsTarget()
	{
		if (pBuffer)
		{
			if (pDepthBuffer)
			{
				// render target and depth buffer
				DXDevice::GetContext().OMSetRenderTargets(1u, pTargetView.GetAddressOf(), pDepthStencilView.Get());
				DXDevice::GetContext().OMSetDepthStencilState(pDepthStencilState.Get(), stencilRef);
			}
			else
			{
				// render target only
				DXDevice::GetContext().OMSetRenderTargets(1u, pTargetView.GetAddressOf(), nullptr);
			}
		}
		else if (pDepthBuffer)
		{
			// depth buffer only
			DXDevice::GetContext().OMSetRenderTargets(0u, nullptr, pDepthStencilView.Get());
			DXDevice::GetContext().OMSetDepthStencilState(pDepthStencilState.Get(), stencilRef);
		}
	}

	void Offscreen::DrawToBackBuffer()
	{
		E2_ASSERT(pBufferResourceView.Get() != nullptr, "Binding an offscreen without a render target to bind");

		DXDevice::Get().SetBackBufferAsRenderTargetNoDepthCheck();
		Bind(); // bind this as resource
		Common.ForDrawToBackBuffer.pVS->Bind();
		pPS->Bind();
		Common.ForDrawToBackBuffer.pVB->BindAndDraw();
		Unbind();
		DXDevice::Get().SetBackBufferAsRenderTarget();
	}

	void Offscreen::Clear()
	{
		constexpr float clearColor[4] = { 0.0f,0.0f,0.0f,0.0f };

		if (pTargetView)
			DXDevice::GetContext().ClearRenderTargetView(pTargetView.Get(), clearColor);

		if (pDepthStencilView)
			DXDevice::GetContext().ClearDepthStencilView(pDepthStencilView.Get(), D3D11_CLEAR_FLAG::D3D11_CLEAR_DEPTH, 1.0f, 0u);
	}

	void Offscreen::Release()
	{
		ReleaseBuffer();
		ReleaseDepthBuffer();
	}

	void Offscreen::Configure()
	{
		if (descriptor.hasRenderTarget)
			InitialiseBuffer();

		if (descriptor.hasDepthBuffer)
			InitialiseDepthBuffer();
	}

	void Offscreen::Reconfigure()
	{
		Release();
		Configure();
	}

	void Offscreen::OnImgui()
	{
		if (ImGui::TreeNode("Offscreen target"))
		{
			ImGui::Text("Slot %i, width %i, height %i", descriptor.slot, width, height);

			if (ImGui::Checkbox("Render Target", &descriptor.hasRenderTarget))
			{
				if (descriptor.hasRenderTarget)
					InitialiseBuffer();
				else
					ReleaseBuffer();
			}

			if (ImGui::Checkbox("Depth Buffer", &descriptor.hasDepthBuffer))
			{
				if (descriptor.hasDepthBuffer)
					InitialiseDepthBuffer();
				else
					ReleaseDepthBuffer();
			}

			if (ImGui::BeginCombo("Filter", pixelShaderName.c_str()))
			{
				for (auto& kv : pixelShaders)
				{
					bool isSelected = (pixelShaderName == kv.first);
					if (ImGui::Selectable(kv.first.c_str(), isSelected)) { pPS = kv.second; pixelShaderName = kv.first; }
					if (isSelected) ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}

			if (descriptor.hasRenderTarget && ImGui::TreeNode("Render target"))
			{
				ImGui::Checkbox("Show", &subDisplay.show);
				if (ImGui::DragFloat2("Left,Top", subDisplay.leftTop, 0.05f, -1.0f, 1.0f)) InitialiseSubDisplayVB();
				if (ImGui::DragFloat("Size", &subDisplay.size, 0.05f, 0.1f, 2.0f)) InitialiseSubDisplayVB();
				ImGui::TreePop();
			}

			if (descriptor.hasDepthBuffer && ImGui::TreeNode("Depth buffer"))
			{
				ImGui::Checkbox("Show", &subDisplayDepthBuffer.show);
				ImGui::Checkbox("Raw pixels", &subDisplayDepthBuffer.displayRaw);
				if (ImGui::DragFloat2("Left,Top", subDisplayDepthBuffer.leftTop, 0.05f, -1.0f, 1.0f)) InitialiseSubDisplayDepthBufferVB();
				if (ImGui::DragFloat("Size", &subDisplayDepthBuffer.size, 0.05f, 0.1f, 2.0f)) InitialiseSubDisplayDepthBufferVB();
				ImGui::TreePop();
			}

			ImGui::TreePop();
		}
	}

	void Offscreen::InitialiseBuffer()
	{
		HRESULT hr;

		D3D11_TEXTURE2D_DESC bufferTexureDesc = DXDevice::Get().GetBackBufferTextureDesc();  // default to the back buffer desc
		bufferTexureDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_RENDER_TARGET | D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE;
		if (descriptor.unorderedAccess) bufferTexureDesc.BindFlags |= D3D11_BIND_FLAG::D3D11_BIND_UNORDERED_ACCESS;

		if (descriptor.DXGIFormat >= 0) bufferTexureDesc.Format = (DXGI_FORMAT)descriptor.DXGIFormat;
		// else defaults to back buffer format

		bufferTexureDesc.Width = width;
		bufferTexureDesc.Height = height;

		hr = DXDevice::GetDevice().CreateTexture2D(&bufferTexureDesc, nullptr, &pBuffer);

		E2_ASSERT_HR(hr, "CreateTexture2D for offscreen failed");

		hr = DXDevice::GetDevice().CreateRenderTargetView(pBuffer.Get(), nullptr, &pTargetView);

		E2_ASSERT_HR(hr, "CreateRenderTargetView for offscreen failed");

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = bufferTexureDesc.Format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION::D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = 1;

		hr = DXDevice::GetDevice().CreateShaderResourceView(pBuffer.Get(), &srvDesc, &pBufferResourceView);

		E2_ASSERT_HR(hr, "CreateShaderResourceView failed for buffer");

		D3D11_SAMPLER_DESC sampDesc = {};
		sampDesc.Filter = D3D11_FILTER::D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_CLAMP;
		sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_CLAMP;
		sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_CLAMP;

		hr = DXDevice::GetDevice().CreateSamplerState(&sampDesc, &pBufferSamplerState);

		E2_ASSERT_HR(hr, "CreateSamplerState failed");
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
		dtDesc.Format = DXGI_FORMAT::DXGI_FORMAT_R32_TYPELESS; // DXGI_FORMAT::DXGI_FORMAT_D32_FLOAT - cannot use this and bind as a shader resource
		dtDesc.SampleDesc.Count = 1u;
		dtDesc.SampleDesc.Quality = 0u;
		dtDesc.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
		dtDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE; // so we can also draw it
		if (descriptor.unorderedAccess) dtDesc.BindFlags |= D3D11_BIND_FLAG::D3D11_BIND_UNORDERED_ACCESS;
		dtDesc.CPUAccessFlags = 0u;
		dtDesc.MiscFlags = 0u;

		hr = DXDevice::GetDevice().CreateTexture2D(&dtDesc, nullptr, &pDepthBuffer);

		E2_ASSERT_HR(hr, "CreateTexture2D failed for DepthStencil");

		D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
		dsvDesc.Format = DXGI_FORMAT::DXGI_FORMAT_D32_FLOAT; // dtDesc.Format - use the correct format rather than typeless
		dsvDesc.ViewDimension = D3D11_DSV_DIMENSION::D3D11_DSV_DIMENSION_TEXTURE2D;
		dsvDesc.Flags = 0;
		dsvDesc.Texture2D.MipSlice = 0u;

		hr = DXDevice::GetDevice().CreateDepthStencilView(pDepthBuffer.Get(), &dsvDesc, &pDepthStencilView);

		E2_ASSERT_HR(hr, "CreateDepthStencilView failed for DepthStencil");

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = DXGI_FORMAT::DXGI_FORMAT_R32_FLOAT;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION::D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = 1;

		hr = DXDevice::GetDevice().CreateShaderResourceView(pDepthBuffer.Get(), &srvDesc, &pDepthBufferResourceView);

		E2_ASSERT_HR(hr, "CreateShaderResourceView failed for depth buffer");

		D3D11_SAMPLER_DESC sampDesc = {};
		sampDesc.Filter = D3D11_FILTER::D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_CLAMP;
		sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_CLAMP;
		sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_CLAMP;

		hr = DXDevice::GetDevice().CreateSamplerState(&sampDesc, &pDepthBufferSamplerState);

		E2_ASSERT_HR(hr, "CreateSamplerState failed");
	}

	void Offscreen::ReleaseBuffer()
	{
		pBuffer.Reset();
		pTargetView.Reset();
		pBufferResourceView.Reset();
		pBufferSamplerState.Reset();
	}

	void Offscreen::ReleaseDepthBuffer()
	{
		pDepthStencilView.Reset();
		pDepthBuffer.Reset();
		pDepthStencilState.Reset();
		pDepthBufferResourceView.Reset();
		pDepthBufferSamplerState.Reset();
	}

	std::shared_ptr<VertexBuffer> Offscreen::CreateVertexBuffer(float left, float top, float right, float bottom)
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

		auto vb = std::make_shared<VertexBuffer>();
		vb->Initialise<Vertex>(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, verticies);
		return vb;
	}

	void Offscreen::InitialiseSubDisplayVB()
	{
		float left = subDisplay.leftTop[0];
		float top = subDisplay.leftTop[1];
		float right = subDisplay.leftTop[0] + subDisplay.size;
		float bottom = subDisplay.leftTop[1] - subDisplay.size;

		subDisplay.pVB = CreateVertexBuffer(left, top, right, bottom);
	}

	void Offscreen::InitialiseSubDisplayDepthBufferVB()
	{
		float left = subDisplayDepthBuffer.leftTop[0];
		float top = subDisplayDepthBuffer.leftTop[1];
		float right = subDisplayDepthBuffer.leftTop[0] + subDisplayDepthBuffer.size;
		float bottom = subDisplayDepthBuffer.leftTop[1] - subDisplayDepthBuffer.size;

		subDisplayDepthBuffer.pVB = CreateVertexBuffer(left, top, right, bottom);
	}

	void Offscreen::ShowSubDisplay()
	{
		if (pBufferResourceView && subDisplay.show)
		{
			DXDevice::Get().SetBackBufferAsRenderTargetNoDepthCheck();

			// bind buffer as resource
			BindBuffer(descriptor.slot);
			
			Common.ForDrawToSubDisplay.pVS->Bind();
			Common.ForDrawToSubDisplay.pPSBuffer->Bind();
			subDisplay.pVB->BindAndDraw();
			DXDevice::Get().ClearShaderResource(descriptor.slot); // Unbind();
			DXDevice::Get().SetBackBufferAsRenderTarget();
		}

		if (pDepthBufferResourceView && subDisplayDepthBuffer.show)
		{
			DXDevice::Get().SetBackBufferAsRenderTargetNoDepthCheck();

			BindDepthBuffer(descriptor.slot);

			Common.ForDrawToSubDisplay.pVS->Bind();
			if (subDisplayDepthBuffer.displayRaw)
				Common.ForDrawToSubDisplay.pPSDepthBufferRaw->Bind();
			else
				 Common.ForDrawToSubDisplay.pPSDepthBuffer->Bind();
			subDisplayDepthBuffer.pVB->BindAndDraw();
			DXDevice::Get().ClearShaderResource(descriptor.slot); // Unbind();
			DXDevice::Get().SetBackBufferAsRenderTarget();
		}
	}

	// Creates some sample filters to use
	// Note: would pass in pixel width/height as a parameter if using seriously
	void Offscreen::AddSampleFilters()
	{
		std::string PSsrc = R"(
			Texture2D tex;
			SamplerState smplr;

			float4 main(float2 texCoord : TexCoord) : SV_TARGET
			{
				return tex.Sample(smplr, texCoord);
			}
		)";

		pixelShaders["Copy"] = PixelShader::CreateFromString(PSsrc);

		PSsrc = R"(
			Texture2D tex;
			SamplerState smplr;

			float4 main(float2 texCoord : TexCoord) : SV_TARGET
			{
				uint width, height;
				tex.GetDimensions(width, height);
				float pw = 1.0 / (float)width;
				float ph = 1.0 / (float)height;

				float4 col = 8.0 * tex.Sample(smplr, texCoord);
				col -= tex.Sample(smplr, texCoord + float2(-pw, 0.0));
				col -= tex.Sample(smplr, texCoord + float2(-pw, ph));
				col -= tex.Sample(smplr, texCoord + float2(0.0, ph));
				col -= tex.Sample(smplr, texCoord + float2(pw, ph));
				col -= tex.Sample(smplr, texCoord + float2(pw, 0.0));
				col -= tex.Sample(smplr, texCoord + float2(pw, -ph));
				col -= tex.Sample(smplr, texCoord + float2(0.0, -ph));
				col -= tex.Sample(smplr, texCoord + float2(-pw, -ph));
				return col;
			}
		)";

		pixelShaders["Edge detection"] = PixelShader::CreateFromString(PSsrc);

		PSsrc = R"(
			Texture2D tex;
			SamplerState smplr;

			float4 main(float2 texCoord : TexCoord) : SV_TARGET
			{
				uint width, height;
				tex.GetDimensions(width, height);
				float pw = 1.0 / (float)width;
				float ph = 1.0 / (float)height;

				float4 col = 5.0 * tex.Sample(smplr, texCoord);
				col -= tex.Sample(smplr, texCoord + float2(-pw, 0.0));
				col -= tex.Sample(smplr, texCoord + float2(0.0, ph));
				col -= tex.Sample(smplr, texCoord + float2(pw, 0.0));
				col -= tex.Sample(smplr, texCoord + float2(0.0, -ph));
				return col;
			}
		)";

		pixelShaders["Sharpen"] = PixelShader::CreateFromString(PSsrc);

		PSsrc = R"(
			Texture2D tex;
			SamplerState smplr;

			float4 main(float2 texCoord : TexCoord) : SV_TARGET
			{
				uint width, height;
				tex.GetDimensions(width, height);
				float pw = 1.0 / (float)width;
				float ph = 1.0 / (float)height;

				float4 col = tex.Sample(smplr, texCoord);
				col += tex.Sample(smplr, texCoord + float2(-pw, 0.0));
				col += tex.Sample(smplr, texCoord + float2(-pw, ph));
				col += tex.Sample(smplr, texCoord + float2(0.0, ph));
				col += tex.Sample(smplr, texCoord + float2(pw, ph));
				col += tex.Sample(smplr, texCoord + float2(pw, 0.0));
				col += tex.Sample(smplr, texCoord + float2(pw, -ph));
				col += tex.Sample(smplr, texCoord + float2(0.0, -ph));
				col += tex.Sample(smplr, texCoord + float2(-pw, -ph));
				col /= 9.0;
				return col;
			}
		)";

		pixelShaders["Box blur 3x3"] = PixelShader::CreateFromString(PSsrc);

		PSsrc = R"(
			Texture2D tex;
			SamplerState smplr;

			float4 main(float2 texCoord : TexCoord) : SV_TARGET
			{
				uint width, height;
				tex.GetDimensions(width, height);
				float pw = 1.0 / (float)width;
				float ph = 1.0 / (float)height;

				float4 col = 4.0 * tex.Sample(smplr, texCoord);
				col += 2.0 * tex.Sample(smplr, texCoord + float2(-pw, 0.0));
				col += tex.Sample(smplr, texCoord + float2(-pw, ph));
				col += 2.0 * tex.Sample(smplr, texCoord + float2(0.0, ph));
				col += tex.Sample(smplr, texCoord + float2(pw, ph));
				col += 2.0 * tex.Sample(smplr, texCoord + float2(pw, 0.0));
				col += tex.Sample(smplr, texCoord + float2(pw, -ph));
				col += 2.0 * tex.Sample(smplr, texCoord + float2(0.0, -ph));
				col += tex.Sample(smplr, texCoord + float2(-pw, -ph));
				col /= 16.0;
				return col;
			}
		)";

		pixelShaders["Gaussian blur 3x3"] = PixelShader::CreateFromString(PSsrc);

		PSsrc = R"(
			Texture2D tex;
			SamplerState smplr;

			float4 main(float2 texCoord : TexCoord) : SV_TARGET
			{
				uint width, height;
				tex.GetDimensions(width, height);
				float pw = 1.0 / (float)width;
				float ph = 1.0 / (float)height;

				float4 col = 0.0;
				col -= tex.Sample(smplr, texCoord + float2(0.0, ph));
				col -= tex.Sample(smplr, texCoord + float2(-pw, ph));
				col -= tex.Sample(smplr, texCoord + float2(-pw, 0.0));
				col += tex.Sample(smplr, texCoord + float2(pw, 0.0));
				col += tex.Sample(smplr, texCoord + float2(pw, -ph));
				col += tex.Sample(smplr, texCoord + float2(0.0, -ph));
				return col;
			}
		)";

		pixelShaders["Emboss 3x3"] = PixelShader::CreateFromString(PSsrc);

		PSsrc = R"(
			Texture2D tex;
			SamplerState smplr;

			float4 main(float2 texCoord : TexCoord) : SV_TARGET
			{
				uint width, height;
				tex.GetDimensions(width, height);
				float pw = 1.0 / (float)width;
				float ph = 1.0 / (float)height;

				float4 col = tex.Sample(smplr, texCoord);
				col += tex.Sample(smplr, texCoord + float2(-pw, ph));
				col += tex.Sample(smplr, texCoord + float2(pw, -ph));
				col += tex.Sample(smplr, texCoord + float2(-pw * 2, ph));
				col += tex.Sample(smplr, texCoord + float2(pw * 2, -ph));
				col += tex.Sample(smplr, texCoord + float2(-pw * 3, ph));
				col += tex.Sample(smplr, texCoord + float2(pw * 3, -ph));
				return col / 7.0;
			}
		)";

		pixelShaders["Motion blur"] = PixelShader::CreateFromString(PSsrc);

		PSsrc = R"(
			Texture2D tex;
			SamplerState smplr;

			float4 main(float2 texCoord : TexCoord) : SV_TARGET
			{
				float3 col = tex.Sample(smplr, texCoord).xyz;
				float grey = (col.x + col.y + col.z) / 3.0;
				return float4(grey, grey, grey, 1.0);
			}
		)";

		pixelShaders["Greyscale 1/3"] = PixelShader::CreateFromString(PSsrc);

		PSsrc = R"(
			Texture2D tex;
			SamplerState smplr;

			float4 main(float2 texCoord : TexCoord) : SV_TARGET
			{
				float4 col = tex.Sample(smplr, texCoord);
				float grey = (col.r * 0.2126, col.g * 0.7152, col.b * 0.0722);
				grey = pow(grey, 1.0/2.2);
				return float4(grey, grey, grey, 1.0);
			}
		)";

		pixelShaders["Greyscale luma gamma corrected"] = PixelShader::CreateFromString(PSsrc);

		PSsrc = R"(
			Texture2D tex;
			SamplerState smplr;

			float4 main(float2 texCoord : TexCoord) : SV_TARGET
			{
				float3 col = pow(tex.Sample(smplr, texCoord).xyz, 1.0/2.2);
				return float4(col, 1.0);
			}
		)";

		pixelShaders["Gamma correction"] = PixelShader::CreateFromString(PSsrc);

		PSsrc = R"(
			Texture2D tex;
			SamplerState smplr;

			float4 main(float2 texCoord : TexCoord) : SV_TARGET
			{
				float4 col = tex.Sample(smplr, texCoord);
				return float4(1.0 - col.xyz, 1.0);
			}
		)";

		pixelShaders["Invert"] = PixelShader::CreateFromString(PSsrc);
	}
}