#include "pch.h"
#include "WireframeMaterial.h"
#include "ShaderCache.h"
#include "VertexLayout.h"

namespace Engine2
{
	namespace Materials
	{
		std::shared_ptr<VertexShader> WireframeMaterial::wireframeVS = nullptr;
		std::shared_ptr<PixelShader>  WireframeMaterial::wireframePS = nullptr;

		WireframeMaterial::WireframeMaterial()
		{
			name = "Wireframe";

			vertexShaderCB.Initialise<TransformMatrix>();
			vertexShaderCB.SetSlot(1);
			pixelShaderCB.Initialise(psData);
			pixelShaderCB.SetSlot(1);

			if (!wireframeVS)
			{
				wireframeVS = std::make_shared<VertexShader>();
				wireframeVS->CompileFromFile(Config::directories["EngineShaderSourceDir"] + "PositionVS.hlsl", VertexLayout::Position::Layout);
			}

			if (!wireframePS)
			{
				wireframePS = std::make_shared<PixelShader>();

				wireframePS->CompileFromSource(R"(
					cbuffer wireframeConst : register (b1)
					{
						float4 lineColor;
					}

					float4 main() : SV_TARGET
					{
						return lineColor;
					}
				)");
			}

			vertexShader = wireframeVS;
			pixelShader  = wireframePS;

			pixelShaderCB.UpdateBuffer(psData);
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
