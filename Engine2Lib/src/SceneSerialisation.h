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

		void SaveSceneNode(Serialisation::Serialiser::Node& node, SceneHierarchy::SceneNode& sceneNode);
		void SaveComponents(Serialisation::Serialiser::Node& node, Entity entity);
		void SaveAssets(Serialisation::Serialiser::Node& node);
	};
}