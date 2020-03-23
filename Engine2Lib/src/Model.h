#pragma once

#include "pch.h"
#include "Common.h"
#include "Mesh.h"
#include "Material.h"

namespace Engine2
{
	class Model
	{
	public:
		Model(std::string name) : name(name) {}

		std::shared_ptr<Mesh> pMesh;
		std::shared_ptr<Material> pMaterial;

		void Bind();

		bool IsActive() { return active; }
		void SetActive(bool makeActive = true) { active = makeActive; }

		void OnImgui();

	protected:
		std::string name;
		bool active = true;
	};
}