#pragma once
#include "Resources.h"

namespace Engine2
{
	class Mesh
	{
	public:
		Mesh() = default;
		Mesh(const std::string& name) : name(name) {}

		virtual ~Mesh() = default;

		inline void BindAndDraw() { drawable->BindAndDraw(); }
		inline void Bind() { drawable->Bind(); }
		inline void Draw() { drawable->Draw(); }

		void OnImgui();

		bool IsValid() { return drawable != nullptr; }

		void SetDrawable(std::shared_ptr<Drawable> vertexBuffer) { drawable = vertexBuffer; }

		void SetName(std::string str) { name = str; }
		const std::string& Name() const { return name; }

	protected:
		std::string name;
		std::shared_ptr<Drawable> drawable;
	};
}
