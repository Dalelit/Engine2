#include "pch.h"
#include "MaterialsManager.h"
#include "Materials/StandardMaterial.h"
#include "Materials/PBRMaterial.h"
#include "Materials/LowPolyMaterial.h"

namespace Engine2
{
	std::unique_ptr<MaterialsManager> MaterialsManager::instance = std::make_unique<MaterialsManager>();

	void MaterialsManager::OnImgui()
	{
		static char materialName[512] = "";

		ImGui::InputText("Material Name", materialName, sizeof(materialName));
		if (ImGui::BeginCombo("Create material of type", nullptr))
		{
			ImGui::Selectable("");
			if (ImGui::Selectable("Standard"))
			{
				std::string name(materialName);
				if (!name.empty() && !assets.Exists(name))
				{
					auto mat = std::make_shared<Materials::StandardMaterial>(name);
					assets.CreateAsset(name, mat);
				}
			}
			if (ImGui::Selectable("PBR"))
			{
				std::string name(materialName);
				if (!name.empty() && !assets.Exists(name))
				{
					auto mat = std::make_shared<Materials::PBRMaterial>(name);
					assets.CreateAsset(name, mat);
				}
			}
			if (ImGui::Selectable("LowPoly"))
			{
				std::string name(materialName);
				if (!name.empty() && !assets.Exists(name))
				{
					auto mat = std::make_shared<Materials::LowPolyMaterial>(name);
					assets.CreateAsset(name, mat);
				}
			}
			ImGui::EndCombo();
		}
		ImGui::Separator();

		for (auto& [name, asset] : assets.Map())
		{
			if (ImGui::TreeNode(name.c_str()))
			{
				(*asset)->OnImgui(); // to do: nasty ptr ptr
				ImGui::TreePop();
			}
		}
	}
}
