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
		if (wireframe) DXDevice::Get().SetWireframeRenderState();

		for (auto& n : nodes) n->Render();

		if (wireframe) DXDevice::Get().SetDefaultRenderState();
	}

	void Model::OnImgui()
	{
		if (ImGui::TreeNode(name.c_str()))
		{
			ImGui::Checkbox("Active", &active);
			ImGui::Checkbox("Wireframe", &wireframe);
			for (auto& n : nodes) n->OnImgui();
			ImGui::TreePop();
		}
	}

	void ModelEntities::Draw()
	{
		if (wireframe) DXDevice::Get().SetWireframeRenderState();

		for (auto& e : instances.instances)
		{
			if (e.IsActive())
			{
				e.LoadTransformT(instances.vsConstantBuffer.data.transform, instances.vsConstantBuffer.data.transformRotation);
				instances.vsConstantBuffer.Bind();
				for (auto& n : nodes) n->Render();
			}
		}

		if (wireframe) DXDevice::Get().SetDefaultRenderState();
	}

	void ModelEntities::OnImgui()
	{
		if (ImGui::TreeNode(name.c_str()))
		{
			ImGui::Checkbox("Active", &active);
			ImGui::Checkbox("Wireframe", &wireframe);
			for (auto& n : nodes) n->OnImgui();

			if (ImGui::TreeNode("Entities"))
			{
				for (auto& e : instances.instances) e.OnImgui();
				ImGui::TreePop();
			}

			ImGui::TreePop();
		}
	}

}
