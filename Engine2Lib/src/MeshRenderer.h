#pragma once
#include "Mesh.h"
#include "Materials/StandardMaterial.h"
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

		bool IsValid() { return mesh && mesh->IsValid() && material; }

		void OnImgui();
		void Serialise(Serialisation::INode& node);

		void SetMesh(std::shared_ptr<Mesh>& srcMesh, Asset* srcAsset = nullptr);
		void ClearMesh();

		std::shared_ptr<Mesh> mesh;
		Asset* meshAsset = nullptr;
		std::shared_ptr<Material> material;
		Asset* materialAsset = nullptr;

	protected:
	};
}