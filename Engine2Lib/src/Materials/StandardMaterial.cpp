#include "pch.h"
#include "StandardMaterial.h"
#include "ShaderCache.h"
#include "VertexLayout.h"

namespace Engine2
{
	namespace Materials
	{
		bool StandardPSData::OnImgui()
		{
			bool changed = false;

			changed |= ImGui::ColorEdit3("Ambient", &ambient.x);
			changed |= ImGui::ColorEdit3("Diffuse", &diffuse.x);
			changed |= ImGui::ColorEdit3("Specular", &specular.x);
			if (ImGui::DragFloat("Exponent", &specularExponent))
			{
				if (specularExponent < 0.001f) specularExponent = 0.001f;
				changed = true;
			}
			changed |= ImGui::ColorEdit3("Emission", &emission.x);

			return changed;
		}

		StandardMaterial::StandardMaterial(const std::string& name)
		{
			this->name = name;

			vertexShaderCB.Initialise<TransformMatrix>();
			vertexShaderCB.SetSlot(1);
			vertexShader = ShaderCache::GetVertexShader(Config::directories["EngineShaderSourceDir"] + "PositionNormalVS.hlsl", VertexLayout::PositionNormalColor::Layout);

			pixelShaderCB.Initialise(psData);
			pixelShaderCB.SetSlot(1);
			pixelShader = ShaderCache::GetPixelShader(Config::directories["EngineShaderSourceDir"] + "PositionNormalPS.hlsl");

			//texture = nullptr;
		}

		void StandardMaterial::OnImgui()
		{
			if (ImGui::TreeNode(this, "Standard Material"))
			{
				ImGui::Text("%s", name.c_str());
				if (psData.OnImgui()) pixelShaderCB.UpdateBuffer(psData);

				if (texture) texture->OnImgui();
				else ImGui::Text("No texture");

				if (ImGui::TreeNode("Info"))
				{
					vertexShader->OnImgui();
					pixelShader->OnImgui();
					ImGui::TreePop();
				}

				ImGui::TreePop();
			}
		}

		std::shared_ptr<Material> StandardMaterial::Clone()
		{
			auto pClone = std::shared_ptr<StandardMaterial>();

			pClone->name = name + " clone";

			pClone->vertexShaderCB.SetSlot(vertexShaderCB.GetSlot());
			pClone->vertexShader = vertexShader;

			pClone->psData = psData;
			pClone->pixelShaderCB.SetSlot(pixelShaderCB.GetSlot());
			pClone->pixelShaderCB.UpdateBuffer(pClone->psData);
			pClone->pixelShader = pixelShader;

			pClone->texture = texture;

			return pClone;
		}
	}
}