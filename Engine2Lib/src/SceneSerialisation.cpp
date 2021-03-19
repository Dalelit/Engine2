#include "pch.h"
#include "SceneSerialisation.h"
#include "Components.h"
#include "Transform.h"
#include "MeshRenderer.h"
#include "RigidBody.h"
#include "Collider.h"
#include "Particles.h"
#include "Lights.h"
#include "OffscreenOutliner.h"
#include "ScriptComponent.h"
#include "Particles.h"
#include "Gizmo.h"

namespace Engine2
{
	bool SceneSerialisation::LoadScene(const std::string& filename)
	{
		auto loader = Serialisation::LoadSerialiserStream(filename);

		if (!loader.NextLine()) return false;

		if (loader.Spaces() == 0 && loader.Name() == "Assets")
		{
			while (loader.NextLine() && loader.Spaces() != 0);
			// to do
		}

		if (loader.Spaces() == 0 && loader.Name() == "SceneInformation")
		{
			while (loader.NextLine() && loader.Spaces() != 0);
			// to do
		}

		if (loader.Spaces() == 0 && loader.Name() == "SceneHierarchy")
		{
			int currentSpace = 0;

			while (loader.NextLine() && loader.Spaces() != 0)
			{
				while (loader.Spaces() == 2 && loader.Name() == "Entity")
				{
					auto e = m_scene.CreateEntity();
					LoadEntity(2, loader, e);
				}
			}
		}

		return true;
	}

	void SceneSerialisation::LoadEntity(int spaces, Serialisation::LoadSerialiserStream& loader, Entity& entity)
	{
		loader.NextLine();

		while (loader.Spaces() == spaces + 2)
		{
			if (loader.Name() == "Entity")
			{
				E2_ASSERT(false, "Have not set up to load nested entities yet");
			}
			else if (loader.Name() == "Scripts")
			{
				auto sc = entity.AddComponent<ScriptComponent>();

				loader.NextLine();

				while (loader.Spaces() == spaces + 4)
				{
					std::string scriptName = loader.Name();
					GetAttributes(spaces + 6, loader);
					auto readNode = Serialisation::ReadNode(m_attributes);

					sc->SetEntity(entity); // to do: shouldn't need to do this.
					auto script = sc->CreateInstance(scriptName);
					script->Serialise(readNode);
				}
			}
			else
			{
				std::string componentName = loader.Name();
				GetAttributes(spaces + 4, loader);
				auto readNode = Serialisation::ReadNode(m_attributes);

				if (componentName == "EntityInfo") entity.GetComponent<EntityInfo>()->Serialise(readNode);
				else if (componentName == "Transform") entity.GetComponent<Transform>()->Serialise(readNode);
				else if (componentName == "RigidBody") entity.AddComponent<RigidBody>()->Serialise(readNode);
				else if (componentName == "Collider") entity.AddComponent<Collider>()->Serialise(readNode);
				else if (componentName == "Camera") entity.AddComponent<Camera>()->Serialise(readNode);
				else if (componentName == "ParticleEmitter") entity.AddComponent<ParticleEmitter>()->Serialise(readNode);
				else if (componentName == "Gizmo") entity.AddComponent<Gizmo>()->Serialise(readNode);
				else if (componentName == "OffscreenOutliner") entity.AddComponent<OffscreenOutliner>()->Serialise(readNode);
			}
		}
	}

	void SceneSerialisation::GetAttributes(int spaces, Serialisation::LoadSerialiserStream& loader)
	{
		m_attributes.clear();

		while (loader.NextLine() && loader.Spaces() == spaces)
		{
			m_attributes[loader.Name()] = loader.Value();
		}
	}

	bool SceneSerialisation::SaveScene(const std::string& filename)
	{
		Serialisation::SaveSerialiser out(filename);

		out.Comment(" Add timestamp");

		{
			auto node = out.SaveNode("Assets");
			SaveAssets(node);
		}

		{
			auto node = out.SaveNode("SceneInformation");
			node.Comment("To do");
		}

		{
			auto node = out.SaveNode("SceneHierarchy");
			for (auto& sceneNode : m_scene.hierarchy.sceneHierarchy)
			{
				SaveSceneNode(node, sceneNode);
			}
		}

		return true;
	}

	void SceneSerialisation::SaveSceneNode(Serialisation::WriteNode& node, SceneHierarchy::SceneNode& sceneNode)
	{
		auto entNode = node.ChildNode("Entity");
		SaveComponents(entNode, m_scene.GetEntity(sceneNode.id));

		for (auto& childNode : sceneNode.children)
		{
			SaveSceneNode(entNode, childNode);
		}
	}

	void SceneSerialisation::SaveComponents(Serialisation::WriteNode& node, Entity entity)
	{
		SaveComponent<EntityInfo>(node, entity, "EntityInfo");
		SaveComponent<Transform>(node, entity, "Transform");
		SaveComponent<RigidBody>(node, entity, "RigidBody");
		SaveComponent<Collider>(node, entity, "Collider");
		SaveComponent<Camera>(node, entity, "Camera");
		SaveComponent<ParticleEmitter>(node, entity, "ParticleEmitter");
		SaveComponent<Gizmo>(node, entity, "Gizmo");
		SaveComponent<OffscreenOutliner>(node, entity, "OffscreenOutliner");

		if (entity.HasComponent<ScriptComponent>())
		{
			auto scriptsNode = node.ChildNode("Scripts");
			auto scripts = entity.GetComponent<ScriptComponent>();
			for (auto& s : scripts->Scripts())
			{
				auto n = scriptsNode.ChildNode(s->Name().c_str());
				s->Serialise(n);
			}
		}

		//if (entity.HasComponent<MeshRenderer>())
		//{
		//	auto c = entity.GetComponent<MeshRenderer>();
		//	auto n = node.SubNode("MeshRenderer");
		//	n.Store("mesh", c->mesh->Name());
		//	n.Store("material", c->material->Name());
		//}
		//if (entity.HasComponent<PointLight>())
		//{
		//	auto c = entity.GetComponent<PointLight>();
		//	auto n = node.SubNode("PointLight");
		//	n.Store("color", c->color);
		//}
		//if (entity.HasComponent<OffscreenOutliner>())
		//{
		//	auto c = entity.GetComponent<OffscreenOutliner>();
		//	auto n = node.SubNode("OffscreenOutliner");
		//	n.Store("outlineScale", c->GetOutlineScale());
		//	n.Store("outlineColor", c->GetOutlineColor());
		//}
	}

	void SceneSerialisation::SaveAssets(Serialisation::WriteNode& node)
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