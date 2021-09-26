#include "pch.h"
#include "LowPolyMaterial.h"
#include "ShaderCache.h"
#include "VertexLayout.h"

namespace Engine2
{
	namespace Materials
	{
		bool LowPolyPSData::OnImgui()
		{
			bool changed = false;

			changed |= ImGui::ColorEdit3("Base color", &baseColor.x);

			return changed;
		}

		LowPolyMaterial::LowPolyMaterial(const std::string& name)
		{
			this->name = name;

			vertexShaderCB.Initialise<TransformMatrix>();
			vertexShaderCB.SetSlot(1);
			vertexShader = ShaderCache::GetVertexShader(Config::directories["EngineShaderSourceDir"] + "PositionNormalVS.hlsl", VertexLayout::PositionNormalColor::Layout);

			pixelShaderCB.Initialise(psData);
			pixelShaderCB.SetSlot(1);
			pixelShader = ShaderCache::GetPixelShader(Config::directories["EngineShaderSourceDir"] + "LowPolyPS.hlsl");

			//texture = nullptr;
		}

		void LowPolyMaterial::OnImgui()
		{
			//if (ImGui::TreeNode(this, name.c_str()))
			{
				if (psData.OnImgui())
					pixelShaderCB.UpdateBuffer(psData);

				OnImguiCommon();

				//ImGui::TreePop();
			}
		}

		std::shared_ptr<Material> LowPolyMaterial::Clone()
		{
			E2_ASSERT(false, "Not implemented");
			return std::shared_ptr<Material>();
		}
	}
}
