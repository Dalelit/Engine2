#pragma once
#include "Mesh.h"
#include "MeshComponent.h"
#include "Material.h"
#include "Serialiser.h"
#include "AssetManager.h"
#include "Components.h"

namespace Engine2
{
	class MeshRenderer
	{
	public:

		void BindAndDraw();
		void ShadowBindAndDraw();

		bool IsValid() { return mesh && mesh->IsValid() && material && material->IsValid(); }

		void OnImgui();
		void Serialise(Serialisation::INode& node);

		void SetMesh(std::shared_ptr<Mesh>& srcMesh, Asset* srcAsset = nullptr);
		void ClearMesh();

		std::shared_ptr<Mesh> mesh;
		std::shared_ptr<MeshComponent> meshComponent;
		Asset* meshAsset = nullptr;
		std::shared_ptr<Material> material;
		Asset* materialAsset = nullptr;

	protected:
	};
}