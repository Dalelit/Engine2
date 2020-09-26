#pragma once

#include "pch.h"
#include "Common.h"
#include "VertexBuffer.h"
#include "Instances.h"

namespace Engine2
{
	class RenderNode
	{
	public:
		RenderNode(std::string name) : name(name) {}
		~RenderNode() { for (auto n : children) delete n; }

		void Render();

		RenderNode* AddRenderNode(std::string name);

		void AddBindable(std::shared_ptr<Bindable> pBindable, bool unbindAfterDraw = false)
		{
			resources.push_back(pBindable);
			if (unbindAfterDraw) resourcesToUnbind.push_back(pBindable.get());
		}

		inline void SetDrawable(std::shared_ptr<Drawable> d) { drawable = d; }
		inline Drawable& GetDrawable() { return *drawable; }
		inline bool HasDrawable() { return drawable != nullptr; }

		bool IsActive() { return active; }
		void SetActive(bool makeActive = true) { active = makeActive; }

		void RenderDrawablesOnly() { if (drawable) drawable->BindAndDraw(); for (auto n : children) n->RenderDrawablesOnly(); }

		void OnImgui();

	protected:
		std::string name;
		bool active = true;
		std::vector<std::shared_ptr<Bindable>> resources;
		std::vector<Bindable*> resourcesToUnbind;
		std::shared_ptr<Drawable> drawable = nullptr;
		std::vector<RenderNode*> children;
	};

	class Model
	{
	public:
		Model(std::string name) : name(name) {}
		virtual ~Model() { for (auto n : nodes) delete n; }

		RenderNode* AddRenderNode(std::string name);
		std::vector<RenderNode*>& GetRenderNodes() { return nodes; }

		virtual void Draw();

		bool IsActive() { return active; }
		void SetActive(bool makeActive = true) { active = makeActive; }

		void RenderDrawablesOnly() { for (auto n : nodes) n->RenderDrawablesOnly(); }

		virtual void OnImgui();

		const std::string& GetName() const { return name; }

	protected:
		std::string name;
		bool active = true;
		bool wireframe = false;
		std::vector<RenderNode*> nodes;
	};

	class ModelEntities : public Model
	{
	public:
		ModelEntities(std::string name) : Model(name) {}

		void Draw() override;
		void OnImgui() override;

		Instances instances;
	};
}