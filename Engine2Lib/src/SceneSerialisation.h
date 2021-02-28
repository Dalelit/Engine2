#pragma once
#include "Scene.h"
#include "Serialiser.h"

namespace Engine2
{
	class SceneSerialisation
	{
	public:
		SceneSerialisation(Scene& scene) : m_scene(scene) {}
		
		bool LoadScene(const std::string& filename);
		bool SaveScene(const std::string& filename);

	protected:
		Scene& m_scene;

		void SaveSceneNode(Serialisation::WriteNode& node, SceneHierarchy::SceneNode& sceneNode);
		void SaveComponents(Serialisation::WriteNode& node, Entity entity);
		void SaveAssets(Serialisation::WriteNode& node);

		void LoadEntity(int spaces, Serialisation::LoadSerialiserStream& loader, Entity& entity);
		void GetAttributes(int spaces, Serialisation::LoadSerialiserStream& loader);
		std::map<std::string, std::string> m_attributes; // working space;

		template <class T>
		void SaveComponent(Serialisation::WriteNode& node, Entity& entity, const char* componentName)
		{
			if (entity.HasComponent<T>())
			{
				auto c = entity.GetComponent<T>();
				auto n = node.ChildNode(componentName);
				c->Serialise(n);
			}
		}
	};
}