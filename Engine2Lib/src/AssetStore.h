#pragma once
#include <string>
#include <memory>
#include <unordered_map>
#include "common.h"


namespace Engine2
{
	class SceneSerialisation; // for a friend

	template <class T>
	class AssetStore
	{
	public:

		using TSharedPtr = std::shared_ptr<T>;
		using TMap = std::unordered_map<std::string, TSharedPtr>;

		TSharedPtr& CreateAsset(const std::string& name)
		{
			E2_ASSERT(!Exists(name), "Attempting to create an asset that already exists");

			map[name] = std::make_shared<T>(name);
			return GetAsset(name);
		}

		template <class OVERLOADEDTYPE, typename... ARGS>
		TSharedPtr& CreateAsset(const std::string& name, ARGS... args)
		{
			E2_ASSERT(!Exists(name), "Attempting to create an asset that already exists");

			map[name] = std::make_shared<OVERLOADEDTYPE>(args...);
			return GetAsset(name);
		}

		// Note: Does not check it exists.
		TSharedPtr& operator[](const std::string& name)
		{
			return map.at(name);
		}

		// Note: Does not check it exists.
		inline TSharedPtr& GetAsset(const std::string& name)
		{
			return map.at(name);
		}

		bool Exists(const std::string& name)
		{
			return map.find(name) != map.end(); // map.count(name) == 1
		}

		void OnImGui()
		{
			for (auto& [k, v] : map)
			{
				if (ImGui::TreeNode(k.c_str()))
				{
					v->OnImgui();
					ImGui::TreePop();
				}
			}
			if (ImGui::TreeNode("Add"))
			{
				static char name[256] = {};

				if (ImGui::Button("Add"))
				{
					std::string nameStr(name);
					if (nameStr.length() > 0 && !Exists(name))
					{
						CreateAsset(name);
						name[0] = '\0';
					}
				}
				ImGui::SameLine();
				ImGui::InputText("Name", name, sizeof(name));

				ImGui::TreePop();
			}
		}

		TSharedPtr OnImguiSelector()
		{
			TSharedPtr result;

			if (ImGui::BeginCombo(typeid(T).name(), nullptr))
			{
				for (auto& [k, v] : map)
				{
					if (ImGui::Selectable(k.c_str()))
					{
						result = v;
					}
				}
				ImGui::EndCombo();
			}
			return result;
		}

		friend SceneSerialisation;

	private:
		TMap map;
	};
}