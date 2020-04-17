#include "pch.h"
#include "Model.h"

namespace Engine2
{
	void RenderNode::Render()
	{
		if (active)
		{
			for (auto& r : resources) r->Bind();
			if (drawable)
			{
				drawable->Bind();
				drawable->Draw();
			}
			for (auto& c : children) c->Render();
			for (auto& r : resourcesToUnbind) r->Unbind();
		}
	}

	RenderNode* RenderNode::AddRenderNode(std::string name)
	{
		RenderNode* ptr = new RenderNode(name);
		children.push_back(ptr);
		return ptr;
	}

	void RenderNode::OnImgui()
	{
		if (ImGui::TreeNode(name.c_str()))
		{
			ImGui::Checkbox("Active", &active);
			if (drawable) drawable->OnImgui();
			for (auto& r : resources) r->OnImgui();
			for (auto& c : children) c->OnImgui();
			ImGui::TreePop();
		}
	}

	RenderNode* Model::AddRenderNode(std::string name)
	{
		RenderNode* ptr = new RenderNode(name);
		nodes.push_back(ptr);
		return ptr;
	}

	void Model::Draw()
	{
		for (auto& n : nodes) n->Render();
	}

	void Model::OnImgui()
	{
		if (ImGui::TreeNode(name.c_str()))
		{
			ImGui::Checkbox("Active", &active);
			for (auto& n : nodes) n->OnImgui();
			ImGui::TreePop();
		}
	}

	void ModelEntities::Draw()
	{
		for (auto& e : entities.instances)
		{
			if (e.IsActive())
			{
				e.LoadTransformT(entities.vsConstantBuffer.data.entityTransform, entities.vsConstantBuffer.data.entityTransformRotation);
				entities.vsConstantBuffer.Bind();
				for (auto& n : nodes) n->Render();
			}
		}
	}

	void ModelEntities::OnImgui()
	{
		if (ImGui::TreeNode(name.c_str()))
		{
			ImGui::Checkbox("Active", &active);
			for (auto& n : nodes) n->OnImgui();

			if (ImGui::TreeNode("Entities"))
			{
				for (auto& e : entities.instances) e.OnImgui();
				ImGui::TreePop();
			}

			ImGui::TreePop();
		}
	}

}
