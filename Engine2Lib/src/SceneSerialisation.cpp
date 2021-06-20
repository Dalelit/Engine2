#include "pch.h"
#include "SceneSerialisation.h"
#include "AssetManager.h"
#include "Components.h"
#include "EntityInfo.h"
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
	bool SceneSerialisation::LoadScene(const std::string& directory, const std::string& filename)
	{
		std::string fullPathFilename = directory + "\\" + filename;
		if (!Util::FileExists(fullPathFilename))
		{
			E2_LOG_WARNING("LoadScene file does not exist: " + fullPathFilename);
			return false;
		}
			
		auto loader = Serialisation::LoadSerialiserStream(fullPathFilename);

		if (!loader.NextLine()) return false;

		if (loader.Spaces() == 0 && loader.Name() == "Assets")
		{
			while (loader.NextLine() && loader.Spaces() != 0)
			{
				if (loader.Name() == "Source")
				{
					AssetManager::Manager().LoadModel(directory, loader.Value());
				}
				else { E2_ASSERT(false, "Expected the source when loading assets"); }
			}
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
				auto childEntity = m_scene.CreateEntity(entity);
				LoadEntity(spaces + 2, loader, childEntity);
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
				else if (componentName == "MeshRenderer") entity.AddComponent<MeshRenderer>()->Serialise(readNode);
				else if (componentName == "Camera") entity.AddComponent<Camera>()->Serialise(readNode);
				else if (componentName == "ParticleEmitter") entity.AddComponent<ParticleEmitter>()->Serialise(readNode);
				else if (componentName == "Gizmo") entity.AddComponent<Gizmo>()->Serialise(readNode);
				else if (componentName == "OffscreenOutliner") entity.AddComponent<OffscreenOutliner>()->Serialise(readNode);
				else if (componentName == "PointLight") entity.AddComponent<PointLight>()->Serialise(readNode);
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

	bool SceneSerialisation::SaveScene(const std::string& directory, const std::string& filename)
	{
		Serialisation::SaveSerialiser out(directory + "\\" + filename);

		out.Comment(" Add timestamp");

		{
			auto node = out.SaveNode("Assets");
			SaveAssets(node);
		}

		{
			auto node = out.SaveNode("SceneInformation");
			SaveSceneInfo(node);
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

	void SceneSerialisation::SaveSceneInfo(Serialisation::WriteNode& node)
	{
		node.Comment("To do");
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
		SaveComponent<MeshRenderer>(node, entity, "MeshRenderer");
		SaveComponent<PointLight>(node, entity, "PointLight");

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
	}

	void SceneSerialisation::SaveAssets(Serialisation::WriteNode& node)
	{
		for (auto [name, asset] : AssetManager::Manager().GetMap())
		{
			node.Attribute("Source", asset.GetSource());
		}
	}
}