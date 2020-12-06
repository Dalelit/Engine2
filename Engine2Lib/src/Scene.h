#pragma once
#include "pch.h"
#include "Events.h"
#include "ConstantBuffer.h"
#include "ECS.h"
#include "Entity.h"
#include "Gizmo.h"
#include "SceneHierarchy.h"
#include "Skybox.h"
#include "Engine2.h"
#include "Camera.h"

namespace Engine2
{
	class SceneSerialisation; // for a friend

	// Used for scene level resources that are commonly used and bound once per frame.
	class Scene
	{
	public:
		Scene();
		~Scene() = default;

		void OnUpdate(float dt);
		void OnRender() { OnRender(mainCameraEntity); }
		void OnRender(EngineECS::EntityId_t cameraEntity);
		void OnApplicationEvent(Engine2::ApplicationEvent& event);

		Entity CreateEntity() { return hierarchy.CreateEntity(); }
		Entity CreateEntity(const std::string& name) { return hierarchy.CreateEntity(name); }
		Entity GetEntity(EngineECS::EntityId_t id) { return hierarchy.GetEntity(id); }

		Entity CreateSceneCamera(const std::string& name, bool makeMainCamera = false);
		void SetMainSceneCarmera(EngineECS::EntityId_t cameraEntityId) { mainCameraEntity = cameraEntityId; }
		Camera& GetMainSceneCamera() { return *hierarchy.GetECSCoordinator().GetComponent<Camera>(mainCameraEntity); }

		void OnImgui();

		struct VSSceneData
		{
			DirectX::XMMATRIX cameraTransform;
		};
		VSConstantBuffer<VSSceneData> vsConstBuffer;

		struct PSSceneData
		{
			DirectX::XMVECTOR CameraPosition;
			DirectX::XMVECTOR pointLightPosition;
			DirectX::XMVECTOR pointLightColor;
		};
		PSConstantBuffer<PSSceneData> psConstBuffer;

		bool LoadModel(const std::string& sourceFilename);

		Skybox& GetSkybox() { return skybox; }

		// returns the previous setting if you want to revert
		inline bool EnableGizmos(bool show = true) { auto current = gizmoEnabled; gizmoEnabled = show; return current; }

		friend SceneSerialisation;

	protected:
		SceneHierarchy hierarchy;
		Skybox skybox;
		EngineECS::EntityId_t mainCameraEntity;

		GizmoRender gizmoRender;
		bool gizmoEnabled = true;

		void UpdateVSSceneConstBuffer(Camera& camera, Transform& transform);
		void UpdatePSSceneConstBuffer(Camera& camera, Transform& transform);

		void RenderMeshes();
		void RenderParticles();
		void RenderGizmos();
		void RenderOutlines();

		void RenderImage(EngineECS::EntityId_t cameraEntity, bool showGizmos = false);

		void CamerasRender();

		void UpdatePhysics(float dt);
		void UpdateTransformMatrix();
		void UpdateParticles(float dt);
		void UpdateCameras();
		void UpdateScripts(float dt);

		void ImGuiScene();
		void ImGuiEntities();
		void ImGuiAssets();
		void ImGuiCameras();

	};

}

