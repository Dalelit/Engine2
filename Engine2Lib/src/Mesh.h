#pragma once
#include "AssetStore.h"
#include "Resources.h"
#include "Shader.h"
#include "AssetLoaders/ObjLoader.h"

namespace Engine2
{
	class Mesh
	{
	public:
		static AssetStore<Mesh> Assets;

		Mesh() = default;
		Mesh(const std::string& name) : name(name) {}

		virtual ~Mesh() = default;

		inline void BindAndDraw() { drawable->BindAndDraw(); }
		inline void Bind() { drawable->Bind(); }
		inline void Draw() { drawable->Draw(); }

		void OnImgui(bool assetInfo = false);

		bool IsValid() { return drawable != nullptr; }

		template <class T, class... ARGS>
		void SetDrawable(ARGS&&... args) { drawable = std::make_shared<T>(std::forward<ARGS>(args)...); }

		void SetName(std::string str) { name = str; }
		const std::string& Name() const { return name; }

	protected:
		std::string name;
		std::shared_ptr<Drawable> drawable;
	};

	class MeshAssetLoader
	{
	public:
		static std::vector<std::string> CreateMeshAsset(AssetLoaders::ObjLoader& loader);
	};
}
