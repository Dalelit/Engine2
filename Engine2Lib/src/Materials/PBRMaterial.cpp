#include "pch.h"
#include "PBRMaterial.h"
#include "ShaderCache.h"
#include "VertexLayout.h"

namespace Engine2
{
	namespace Materials
	{
		bool PBRPSData::OnImgui()
		{
			bool changed = false;

			changed |= ImGui::ColorEdit3("Base color", &baseColor.x);
			changed |= ImGui::DragFloat("Metallic", &metallic, 0.005f, 0.0f, 1.0f);
			changed |= ImGui::DragFloat("Roughness", &roughness, 0.005f, 0.0f, 1.0f);
			changed |= ImGui::DragFloat("Ambient", &ambient, 0.005f, 0.0f, 1.0f);
			changed |= ImGui::DragFloat("Specular", &specular);

			return changed;
		}

		PBRMaterial::PBRMaterial(const std::string& name)
		{
			this->name = name;

			vertexShaderCB.Initialise<TransformMatrix>();
			vertexShaderCB.SetSlot(1);
			vertexShader = ShaderCache::GetVertexShader(Config::directories["EngineShaderSourceDir"] + "PositionNormalVS.hlsl", VertexLayout::PositionNormalColor::Layout);

			pixelShaderCB.Initialise(psData);
			pixelShaderCB.SetSlot(1);
			pixelShader = ShaderCache::GetPixelShader(Config::directories["EngineShaderSourceDir"] + "PBRPS.hlsl", "mainPositionNormal");

			//texture = nullptr;
		}

		void PBRMaterial::OnImgui()
		{
			//if (ImGui::TreeNode(this, name.c_str()))
			{
				if (psData.OnImgui())
					pixelShaderCB.UpdateBuffer(psData);

				OnImguiCommon();

				//ImGui::TreePop();
			}
		}

		std::shared_ptr<Material> PBRMaterial::Clone()
		{
			E2_ASSERT(false, "Not implemented");
			return std::shared_ptr<Material>();
		}
	}
}
