#include "pch.h"
#include "AssetManager.h"

namespace Engine2
{
	bool AssetManager::LoadModel(const std::string& filename)
	{
		return false;
	}

	std::optional<AssetRef> AssetManager::GetAsset(const std::string& assetName)
	{
		auto iter = assets.find(assetName);

		return iter == assets.end() ? std::nullopt
									: std::optional<AssetRef>{ iter->second };
	}
	
	void Asset::OnImgui()
	{
		ImGui::Text("Source: %s", source.c_str());
	}

	void AssetManager::OnImgui()
	{
		if (ImGui::TreeNode("Assets"))
		{
			for (auto [name, asset] : assets)
			{
				ImGui::TreeNode(name.c_str());
				asset.OnImgui();
				ImGui::TreePop();
			}
			ImGui::TreePop();
		}
	}

}