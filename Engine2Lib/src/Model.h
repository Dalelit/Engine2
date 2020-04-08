#pragma once

#include "pch.h"
#include "Common.h"
#include "VertexBuffer.h"
#include "RenderNode.h"
#include "Entity.h"

namespace Engine2
{
	class Model
	{
	public:
		Model(std::string name) : name(name) {}

		std::shared_ptr<Drawable> pMesh;
		std::shared_ptr<RenderNode> pMaterial;

		void OnRender();

		bool IsActive() { return active; }
		void SetActive(bool makeActive = true) { active = makeActive; }

		void OnImgui();

		const std::string& GetName() const { return name; }

		EntityInstances entities;

	protected:
		std::string name;
		bool active = true;
	};
}