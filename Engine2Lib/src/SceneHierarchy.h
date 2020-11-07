#pragma once
#include "ECS.h"
#include "Entity.h"
#include <vector>

namespace Engine2
{
	class SceneSerialisation; // for a friend

	class SceneHierarchy
	{
	public:
		SceneHierarchy() : coordinator(1000) {}

		Entity CreateEntity() { return Entity(NewEntity()->id, coordinator); }
		Entity GetEntity(EngineECS::EntityId_t id) { return Entity(id, coordinator); }

		EngineECS::Coordinator& GetECSCoordinator() { return coordinator; }

		void UpdateTransformMatrix();

		void OnImgui();
		void SelectedEntityOnImgui();

		bool LoadModel(const std::string& sourceFilename);

		friend SceneSerialisation;

	protected:
		EngineECS::Coordinator coordinator;

		struct SceneNode {
			EngineECS::EntityId_t id;
			std::vector<SceneNode> children;

			SceneNode(EngineECS::EntityId_t id) : id(id) {}
		};

		SceneNode* selected = nullptr;
		std::vector<SceneNode> sceneHierarchy;

		SceneNode* NewEntity(SceneNode* parent = nullptr, SceneNode* insertBefore = nullptr);
		void DestroyEntity(SceneNode* parent, SceneNode* node);

		void UpdateTransformMatrix(SceneNode& node, DirectX::XMMATRIX& parentMatrix);

		void SceneNodeOnImGui(SceneNode& node);
		SceneNode* onImguiParent = nullptr;
		SceneNode* addEntityParent = nullptr;
		SceneNode* addEntityInsertBefore = nullptr;
		SceneNode* deleteEntity = nullptr;
		SceneNode* deleteEntityParent = nullptr;
	};
}