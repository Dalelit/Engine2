#pragma once
#include "ECS.h"
#include "Entity.h"
#include <vector>
#include "Components.h"
#include "Transform.h"

namespace Engine2
{
	class SceneSerialisation; // for a friend

	class SceneHierarchy
	{
	public:

		struct SceneNode {
			EngineECS::EntityId_t id;
			std::vector<SceneNode> children;

			SceneNode(EngineECS::EntityId_t id) : id(id) {}
		};

		SceneHierarchy() : coordinator(1000) {}

		Entity CreateEntity(const std::string& name = std::string());
		Entity CreateEntity(Entity& parent, const std::string& name = std::string());
		
		Entity GetEntity(EngineECS::EntityId_t id) { return Entity(id, coordinator); }

		EngineECS::Coordinator& GetECSCoordinator() { return coordinator; }

		void UpdateTransformMatrix();

		void OnImgui();
		void SelectedEntityOnImgui();

		inline bool EntitySelected() { return selected != nullptr; }
		inline const SceneNode* GetSelected() { return selected; }

		void Clear();

		friend SceneSerialisation;

	protected:
		EngineECS::Coordinator coordinator;

		SceneNode* selected = nullptr;
		std::vector<SceneNode> sceneHierarchy;

		SceneNode* FindNode(Entity& entity) { return FindNode(entity, sceneHierarchy); }
		SceneNode* FindNode(Entity& entity, std::vector<SceneNode>& nodes);

		SceneNode* NewEntity(const std::string& name, SceneNode* parent = nullptr, SceneNode* insertBefore = nullptr);
		void CloneEntity(SceneNode* parent, SceneNode* node);
		void DestroyEntity(SceneNode* parent, SceneNode* node);
		void NewEntityModel(const std::string& assetName, SceneNode* parent = nullptr);

		void UpdateTransformMatrix(SceneNode& node, TransformMatrix& parentMatrix);

		void SceneNodeOnImGui(SceneNode& node);
		SceneNode* onImguiParent = nullptr;
		SceneNode* addEntityParent = nullptr;
		SceneNode* addEntityInsertBefore = nullptr;
		SceneNode* deleteEntity = nullptr;
		SceneNode* deleteEntityParent = nullptr;
		SceneNode* cloneEntity = nullptr;
		SceneNode* cloneEntityParent = nullptr;
		SceneNode* addModelParent = nullptr;
		std::string addModelName;
	};
}