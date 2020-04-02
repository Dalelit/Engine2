#pragma once

#include "pch.h"
#include "Common.h"
#include "Shader.h"
#include "ConstantBuffer.h"
#include "Texture.h"

namespace Engine2
{
	class RenderNode
	{
	public:
		RenderNode(std::string name) : name(name) {}
		~RenderNode() = default;
		void Bind()            { for (auto r : resources) r->Bind(); };
		void Unbind()          { for (auto r : resources) r->Unbind(); };
		void UnbindAfterDraw() { for (auto r : resourcesToUnbind) r->Unbind(); };

		void AddBindable(std::shared_ptr<Bindable> pBindable, bool unbindAfterDraw = false)
		{
			resources.push_back(pBindable);
			if (unbindAfterDraw) resourcesToUnbind.push_back(pBindable);
		}

		void OnImgui()
		{
			ImGui::Text(("Material: " + name).c_str());
			for (auto r : resources) r->OnImgui();
		}

	protected:
		std::string name;
		std::vector<std::shared_ptr<Bindable>> resources;
		std::vector<std::shared_ptr<Bindable>> resourcesToUnbind;
	};
}