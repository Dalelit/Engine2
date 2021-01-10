#pragma once
#include <string>
#include <memory>
#include <unordered_map>
#include "common.h"
#include "Util.h"


namespace Engine2
{
	class SceneSerialisation; // for a friend

	template <class T>
	class AssetStore
	{
	public:

		using TSharedPtr = std::shared_ptr<T>;
		using TMap = std::unordered_map<std::string, TSharedPtr>;

		AssetStore()
		{
			displayName = Util::TypeNameClean<T>();
		}

		virtual ~AssetStore() = default;

		template <typename... ARGS>
		TSharedPtr CreateAsset(const std::string& name, ARGS... args)
		{
			E2_ASSERT(!Exists(name), "Attempting to create an asset that already exists");

			map[name] = std::make_shared<T>(args...);
			return GetAsset(name);
		}

		template <class OVERLOADEDTYPE, typename... ARGS>
		TSharedPtr CreateAsset(const std::string& name, ARGS... args)
		{
			E2_ASSERT(!Exists(name), "Attempting to create an asset that already exists");

			map[name] = std::make_shared<OVERLOADEDTYPE>(args...);
			return GetAsset(name);
		}

		TSharedPtr StoreAsset(const std::string& name, TSharedPtr ptr)
		{
			E2_ASSERT(!Exists(name), "Attempting to store an asset that already exists");

			map[name] = ptr;
			return ptr;
		}

		void Clear()
		{
			// to do: check what happens to this loop in release mode
			for (auto& [k, v] : map)
				E2_ASSERT(v.use_count() == 1, "Trying to clear an asset store when an asset is still being referenced");

			map.clear();
		}

		// Note: Does not check it exists.
		TSharedPtr operator[](const std::string& name)
		{
			return GetAsset(name);
		}

		// Note: Does not check it exists.
		inline TSharedPtr GetAsset(const std::string& name)
		{
			auto ptr = map.find(name);
			if (map.find(name) != map.end()) return ptr->second;
			else return nullptr;
		}

		bool Exists(const std::string& name)
		{
			return map.find(name) != map.end(); // map.count(name) == 1
		}

		size_t Size() { return map.size(); }

		void OnImGui()
		{
			for (auto& [k, v] : map)
			{
				if (ImGui::TreeNode(k.c_str()))
				{
					ImGui::Text("Reference count %d", v.use_count());
					v->OnImgui();
					ImGui::TreePop();
				}
			}
		}

		TSharedPtr OnImguiSelector() { return OnImguiSelector(displayName); }

		TSharedPtr OnImguiSelector(const std::string label)
		{
			TSharedPtr result;

			if (ImGui::BeginCombo(label.c_str(), nullptr))
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

		TMap& Map() { return map; }

		friend SceneSerialisation;

	private:
		TMap map;
		std::string displayName;
	};
}