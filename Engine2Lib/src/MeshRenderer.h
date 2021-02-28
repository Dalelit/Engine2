#pragma once
#include "Mesh.h"
#include "Material.h"

namespace Engine2
{
	class MeshRenderer
	{
	public:

		MeshRenderer() : mesh(defaultMesh), material(defaultMaterial) {}

		void BindAndDraw();
		void ShadowBindAndDraw();

		bool IsValid() { return mesh && mesh->IsValid() && material && material->IsValid(); }

		void OnImgui();

		std::shared_ptr<Mesh> mesh;
		std::shared_ptr<Material> material;

		static std::weak_ptr<Mesh> defaultMesh;
		static std::weak_ptr<Material> defaultMaterial;

	protected:
	};
}