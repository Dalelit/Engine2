#include "pch.h"
#include "SceneSerialisation.h"
#include "Components.h"
#include "Transform.h"
#include "MeshRenderer.h"
#include "RigidBody.h"
#include "Particles.h"
#include "Lights.h"
#include "OffscreenOutliner.h"

namespace Engine2
{
	bool SceneSerialisation::LoadScene(const std::string& filename)
	{
		return false;
	}

	bool SceneSerialisation::SaveScene(const std::string& filename)
	{
		Serialisation::Serialiser out(filename);

		out.Comment(" Add timestamp");

		{
			auto node = out.GetNode("Assets");
			SaveAssets(node);
		}

		{
			auto node = out.GetNode("SceneInformation");
			node.Comment("To do");
		}

		{
			auto node = out.GetNode("SceneHierarchy");
			for (auto& sceneNode : m_scene.hierarchy.sceneHierarchy)
			{
				SaveSceneNode(node, sceneNode);
			}
		}

		return true;
	}

	void SceneSerialisation::SaveSceneNode(Serialisation::Serialiser::Node& node, SceneHierarchy::SceneNode& sceneNode)
	{
		auto entNode = node.SubNode("Entity");
		SaveComponents(entNode, m_scene.GetEntity(sceneNode.id));

		for (auto& childNode : sceneNode.children)
		{
			SaveSceneNode(entNode, childNode);
		}
	}

	void SceneSerialisation::SaveComponents(Serialisation::Serialiser::Node& node, Entity entity)
	{
		if (entity.HasComponent<Transform>())
		{
			auto c = entity.GetComponent<Transform>();
			auto n = node.SubNode("Transform");
			n.Store("position", c->position);
			n.Store("scale", c->scale);
			n.Store("rotation", c->rotation);
		}
		if (entity.HasComponent<EntityInfo>())
		{
			auto c = entity.GetComponent<EntityInfo>();
			auto n = node.SubNode("EntityInfo");
			n.Store("tag", c->tag);
		}
		if (entity.HasComponent<RigidBody>())
		{
			auto c = entity.GetComponent<RigidBody>();
			auto n = node.SubNode("RigidBody");
		}
		if (entity.HasComponent<MeshRenderer>())
		{
			auto c = entity.GetComponent<MeshRenderer>();
			auto n = node.SubNode("MeshRenderer");
			n.Store("mesh", c->mesh->Name());
			n.Store("material", c->material->Name());
		}
		if (entity.HasComponent<PointLight>())
		{
			auto c = entity.GetComponent<PointLight>();
			auto n = node.SubNode("PointLight");
			n.Store("color", c->color);
		}
		if (entity.HasComponent<ParticleEmitter>())
		{
			auto c = entity.GetComponent<ParticleEmitter>();
			auto n = node.SubNode("ParticleEmitter");
		}
		if (entity.HasComponent<Gizmo>())
		{
			auto c = entity.GetComponent<Gizmo>();
			auto n = node.SubNode("Gizmo");
			n.Store("type", c->type);
		}
		if (entity.HasComponent<OffscreenOutliner>())
		{
			auto c = entity.GetComponent<OffscreenOutliner>();
			auto n = node.SubNode("OffscreenOutliner");
			n.Store("outlineScale", c->GetOutlineScale());
			n.Store("outlineColor", c->GetOutlineColor());
		}
	}

	void SceneSerialisation::SaveAssets(Serialisation::Serialiser::Node& node)
	{
		//{
		//	auto n = node.SubNode("Meshes");
		//	for (auto [name, mesh] : Mesh::Assets.map)
		//	{
		//		auto m = n.SubNode(name);
		//		m.Store("type", 0);
		//		m.Store("filename", "to do");
		//	}
		//}
		//{
		//	auto n = node.SubNode("Materials");
		//	for (auto [name, mat] : Material::Materials.map)
		//	{
		//		auto m = n.SubNode(name);
		//		if (mat->pixelShader) m.Store("pixelShader", mat->pixelShader->GetName());
		//		if (mat->vertexShader) m.Store("vertexShader", mat->vertexShader->GetName());
		//		//m.Store("vertexShaderCB", mat->vertexShaderCB->);
		//	}
		//}
	}
}