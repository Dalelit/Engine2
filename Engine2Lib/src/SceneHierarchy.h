#pragma once
#include "ECS.h"
#include "Entity.h"
#include <vector>

namespace Engine2
{
	class SceneHierarchy
	{
	public:
		SceneHierarchy() : coordinator(1000) {}

		Entity CreateEntity() { return Entity(NewEntity(), coordinator); }

		EngineECS::Coordinator& GetECSCoordinator() { return coordinator; }

		void OnImGui();
		void SelectedEntityOnImGui();

	protected:
		EngineECS::Coordinator coordinator;

		struct SceneNode {
			EngineECS::EntityId_t id;
			std::vector<SceneNode> children;

			SceneNode(EngineECS::EntityId_t id) : id(id) {}
		};

		SceneNode* selected = nullptr;
		std::vector<SceneNode> sceneHierarchy;

		EngineECS::EntityId_t NewEntity(SceneNode* parent = nullptr, SceneNode* insertBefore = nullptr);
		void DestroyEntity(SceneNode* parent, SceneNode* node);

		void SceneNodeOnImGui(SceneNode& node);
		SceneNode* onImguiParent = nullptr;
		SceneNode* addEntityParent = nullptr;
		SceneNode* addEntityInsertBefore = nullptr;
		SceneNode* deleteEntity = nullptr;
		SceneNode* deleteEntityParent = nullptr;
	};
}