#include "pch.h"
#include "TextureGizmo.h"

namespace Engine2
{
	TextureGizmo::TextureGizmo(UINT textureSlot) : slot(textureSlot)
	{
		CreateVertexBuffer();
		InitialiseShaders();
	}

	void TextureGizmo::OnImgui()
	{
		ImGui::Checkbox("Show texture", &active);

		if (ImGui::DragFloat2("Left, top", leftTop, 0.01f)) SetLeftTop(leftTop[0], leftTop[1]);
		if (ImGui::DragFloat2("Width, Height", widthHeight, 0.01f)) SetWidthHeight(widthHeight[0], widthHeight[1]);
		if (ImGui::Checkbox("Depth texture", &isDepthTexture)) InitialisePixelShader();

		if (active) RenderTexture();
	}

	void TextureGizmo::RenderTexture()
	{
		if (active)
		{
			DXDevice::Get().SetDepthStencilStateDepthDisabled();

			vs.Bind();
			ps.Bind();
			vb.BindAndDraw();

			DXDevice::Get().SetDefaultDepthStencilState();
		}
	}

	void TextureGizmo::SetLeftTop(float left, float top)
	{
		leftTop[0] = std::clamp(left, -1.0f, 1.0f - widthHeight[0]);
		leftTop[1] = std::clamp(top, -1.0f + widthHeight[1], 1.0f);
		CreateVertexBuffer();
	}

	void TextureGizmo::SetWidthHeight(float width, float height)
	{
		widthHeight[0] = std::clamp(width, 0.0f, 1.0f - leftTop[0]);
		widthHeight[1] = std::clamp(height, 0.0f, 1.0f + leftTop[1]);
		CreateVertexBuffer();
	}

	void TextureGizmo::InitialiseShaders()
	{
		std::vector<D3D11_INPUT_ELEMENT_DESC> layout = {
			{"Position", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TexCoord", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
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

		vs.CompileFromSource(VSsrc, layout);

		InitialisePixelShader();
	}

	void TextureGizmo::InitialisePixelShader()
	{
		std::string PSsrc;
		PSsrc = "Texture2D tex : register (t" + std::to_string(slot) + ");\n";
		PSsrc += "SamplerState smplr : register (s" + std::to_string(slot) + ");\n";

		if (isDepthTexture)
		{
			PSsrc += R"(

				float4 main(float2 texCoord : TexCoord) : SV_TARGET
				{
					float depth = tex.Sample(smplr, texCoord);
					return float4(depth, depth, depth, 1.0);
				}
			)";
		}
		else
		{
			PSsrc += R"(

				float4 main(float2 texCoord : TexCoord) : SV_TARGET
				{
					return float4(tex.Sample(smplr, texCoord));
				}
			)";
		}

		ps.CompileFromSource(PSsrc);
	}

	void TextureGizmo::CreateVertexBuffer(float left, float top, float right, float bottom)
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

		vb.Initialise<Vertex>(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, verticies);
	}

}
