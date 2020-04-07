#pragma once
#include "pch.h"
#include "Offscreen.h"
#include "Engine2.h"
#include "Mesh.h"

namespace Engine2
{
	Offscreen::Offscreen(unsigned int slot) :
		Offscreen(Engine::GetDX().CreateOffscreenRenderTarget(), slot)
	{
	}

	Offscreen::Offscreen(unsigned int renderTargetId, unsigned int slot) :
		renderTargetId(renderTargetId), slot(slot)
	{
		CreateResources();
	}

	void Offscreen::Bind()
	{
		Engine::GetDX().BindRenderTargetAsResource(renderTargetId, slot);
	}

	void Offscreen::Unbind()
	{
		Engine::GetDX().UnbindRenderTargetAsResource(renderTargetId, slot);
	}

	void Offscreen::SetAsTarget()
	{
		Engine::GetDX().BindRenderTargetAsTarget(renderTargetId);
	}

	void Offscreen::ClearAsTarget()
	{
		Engine::GetDX().BindBackbufferRenderTarget();
	}

	void Offscreen::Draw()
	{
		Draw(Engine::GetDX().GetBackbufferRenderTargetId());
	}

	void Offscreen::Draw(unsigned int toRenderTargetId)
	{
		Engine::GetDX().BindRenderTargetAsTarget(toRenderTargetId);

		Bind(); // bind this as resource
		pVS->Bind();
		pPS->Bind();
		pDrawable->Bind();
		pDrawable->Draw();
	}

	void Offscreen::CreateResources()
	{
		struct Vertex {
			float position[3];
			float texCoord[2];
		};

		VertexShaderLayout vsLayout = {
			{"Position", DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT},
			{"TexCoord", DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT},
		};

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

		pVS = VertexShader::CreateFromString(VSsrc, vsLayout);
		pPS = PixelShader::CreateFromString(PSsrc);
		pDrawable = std::make_shared<MeshTriangleList<Vertex>>(verticies);
	}
}