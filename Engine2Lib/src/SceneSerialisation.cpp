#include "pch.h"
#include "SceneSerialisation.h"
#include "Components.h"
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

		{
			auto node = out.GetNode("Assets");
			node.Comment("To do");
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
			n.Store("transform", c->transform);
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
			n.Store("velocity", c->velocity);
			n.Store("angularVelocity", c->angularVelocity);
			n.Store("mass", c->mass);
			n.Store("gravity", c->gravity);
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
}