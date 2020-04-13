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
		virtual ~Model() = default;

		std::shared_ptr<Drawable> pMesh;
		std::shared_ptr<RenderNode> pMaterial;

		virtual void Draw();

		bool IsActive() { return active; }
		void SetActive(bool makeActive = true) { active = makeActive; }

		virtual void OnImgui();

		const std::string& GetName() const { return name; }

	protected:
		std::string name;
		bool active = true;
	};

	class ModelEntities : public Model
	{
	public:
		ModelEntities(std::string name) : Model(name) {}

		void Draw() override;
		void OnImgui() override;

		EntityInstances entities;
	};
}