#pragma once
#include "Material.h"
#include "AssetStore.h"

namespace Engine2
{
	class MaterialsManager
	{
	public:

		static inline MaterialsManager& Manager() { return *instance; }

		void OnImgui();

		auto& Store() { return assets; }

	protected:
		static std::unique_ptr<MaterialsManager> instance;
		AssetStore<std::shared_ptr<Material>> assets;
	};
}