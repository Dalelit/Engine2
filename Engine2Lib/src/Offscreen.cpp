#pragma once
#include "pch.h"
#include "Offscreen.h"
#include "DXDevice.h"
#include "VertexBuffer.h"

namespace Engine2
{
	Offscreen::Offscreen(unsigned int slot) :
		Offscreen(DXDevice::Get().CreateOffscreenRenderTarget(), slot)
	{
	}

	Offscreen::Offscreen(unsigned int renderTargetId, unsigned int slot) :
		renderTargetId(renderTargetId), slot(slot)
	{
		CreateResources();
	}

	void Offscreen::Bind()
	{
		DXDevice::Get().BindRenderTargetAsResource(renderTargetId, slot);
	}

	void Offscreen::Unbind()
	{
		DXDevice::Get().UnbindRenderTargetAsResource(renderTargetId, slot);
	}

	void Offscreen::SetAsTarget()
	{
		DXDevice::Get().BindRenderTargetAsTarget(renderTargetId);
	}

	void Offscreen::ClearAsTarget()
	{
		DXDevice::Get().BindBackbufferRenderTarget();
	}

	void Offscreen::Draw()
	{
		Draw(DXDevice::Get().GetBackbufferRenderTargetId());
	}

	void Offscreen::Draw(unsigned int toRenderTargetId)
	{
		//DXDevice::Get().BindRenderTargetAsTarget(toRenderTargetId);
		DXDevice::Get().BindBackbufferNoDepthbuffer(); // to do: hack

		Bind(); // bind this as resource
		pVS->Bind();
		pPS->Bind();
		pDrawable->Bind();
		pDrawable->Draw();
		Unbind();
		DXDevice::Get().BindBackbufferRenderTarget(); // to do: hack?
	}

	void Offscreen::CreateResources()
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
}