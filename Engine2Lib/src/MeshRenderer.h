#pragma once
#include "Mesh.h"
#include "Material.h"

namespace Engine2
{
	class MeshRenderer
	{
	public:

		void BindAndDraw();
		void ShadowBindAndDraw();

		bool IsValid() { return mesh && mesh->IsValid() && material && material->IsValid(); }

		void OnImgui();

		std::shared_ptr<Mesh> mesh;
		std::shared_ptr<Material> material;

	protected:
	};
}