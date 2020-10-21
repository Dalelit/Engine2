#pragma once
#include "AssetStore.h"
#include "Resources.h"
#include "Shader.h"

namespace Engine2
{
	class Mesh
	{
	public:
		static AssetStore<Mesh> Assets;

		Mesh() = default;
		Mesh(const std::string& name) : name(name) {}

		virtual ~Mesh() = default;

		void BindAndDraw();

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
}
