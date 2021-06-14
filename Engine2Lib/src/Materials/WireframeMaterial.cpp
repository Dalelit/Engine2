#include "pch.h"
#include "WireframeMaterial.h"
#include "ShaderCache.h"
#include "VertexLayout.h"

namespace Engine2
{
	namespace Materials
	{
		std::shared_ptr<PixelShader> WireframeMaterial::colorPS = nullptr;

		WireframeMaterial::WireframeMaterial()
		{
			name = "Wireframe";

			vertexShaderCB.SetSlot(1);
			vertexShader = ShaderCache::GetVertexShader(Config::directories["EngineShaderSourceDir"] + "PositionVS.hlsl", VertexLayout::Position::Layout);
			pixelShaderCB.SetSlot(1);

			if (!colorPS)
			{
				colorPS->CompileFromSource(R"(
					cbuffer wireframeConst : register (b1)
					{
						float4 lineColor;
					}

					float4 main(float3 posWS : WSPosition, float3 norWS : WSNormal, float4 col : Color) : SV_TARGET
					{
						return lineColor;
					}
				)");
			}

			pixelShader = colorPS;
		}

		void WireframeMaterial::OnImgui()
		{
			if (ImGui::ColorEdit3("Color", &psData.x)) pixelShaderCB.UpdateBuffer(psData);
		}

		std::shared_ptr<Material> WireframeMaterial::Clone()
		{
			auto pClone = std::shared_ptr<WireframeMaterial>();

			pClone->name = name + " clone";

			pClone->vsData = vsData;
			pClone->vertexShaderCB.SetSlot(vertexShaderCB.GetSlot());
			pClone->vertexShaderCB.UpdateBuffer(pClone->vsData);

			pClone->psData = psData;
			pClone->pixelShaderCB.SetSlot(pixelShaderCB.GetSlot());
			pClone->pixelShaderCB.UpdateBuffer(pClone->psData);

			return pClone;
		}
	}
}
