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
#include "Lights.h"
#include "StructuredBuffer.h"
#include "Physics.h"

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
		inline void OnRender() { OnRender(mainCameraEntity); }
		void OnRender(EngineECS::EntityId_t cameraEntity);
		void OnApplicationEvent(Engine2::ApplicationEvent& event);

		Entity CreateEntity(const std::string& name = std::string()) { return hierarchy.CreateEntity(name); }
		Entity CreateEntity(Entity& parent, const std::string& name = std::string()) { return hierarchy.CreateEntity(parent, name); }
		Entity GetEntity(EngineECS::EntityId_t id) { return hierarchy.GetEntity(id); }

		Entity CreateSceneCamera(const std::string& name, bool makeMainCamera = false);
		void SetMainSceneCarmera(EngineECS::EntityId_t cameraEntityId) { mainCameraEntity = cameraEntityId; }
		Camera& GetMainSceneCamera() { return *hierarchy.GetECSCoordinator().GetComponent<Camera>(mainCameraEntity); }

		void Clear();

		void OnImgui();

		void Start();
		void Stop();
		void Pause();
		void Resume();

		struct VSSceneData
		{
			DirectX::XMMATRIX cameraTransform;
		};
		VSConstantBuffer<VSSceneData> vsConstBuffer;

		struct PSSceneData
		{
			DirectX::XMVECTOR cameraPosition;
			uint32_t numberOfPointLights;
		};
		PSConstantBuffer<PSSceneData> psConstBuffer;

		template <typename T>
		std::shared_ptr<T> SetSkybox() { auto ptr = std::make_shared<T>(); pSkybox = ptr; return ptr; }

		Skybox& GetSkybox() { return *pSkybox; }

		// returns the previous setting if you want to revert
		inline bool EnableGizmos(bool show = true) { auto current = gizmoEnabled; gizmoEnabled = show; return current; }

		friend SceneSerialisation;

	protected:
		struct PointLightBufferData
		{
			DirectX::XMFLOAT3 position;
			DirectX::XMFLOAT3 color;
		};

		SceneHierarchy hierarchy;

		std::shared_ptr<Skybox> pSkybox;
		DirectionalLight sun;
		StructuredBufferLocalData<PointLightBufferData> pointLightsBuffer;

		EngineECS::EntityId_t mainCameraEntity;
		bool running = false;
		bool paused = false;
		Physics physics;

		GizmoRender gizmoRender;
		bool gizmoEnabled = true;
		bool gizmoCollidersEnabled = false;

		Instrumentation::TimerCollection systemTimers;

		void UpdateVSSceneConstBuffer(Camera& camera, Transform& transform);
		void UpdatePSSceneConstBuffer(Camera& camera, Transform& transform);

		void RenderMeshes();
		void RenderParticles();
		void RenderGizmos();
		void RenderOutlines();
		void RenderColliders();

		void RenderImage(EngineECS::EntityId_t cameraEntity, bool mainCamera = true);

		void ShadowPass(EngineECS::EntityId_t viewCameraEntity);

		void CamerasRender();

		void LoadPhysics();
		void ClearPhysics();

		void UpdatePhysics(float dt);
		void UpdateTransformMatrix();
		void UpdateParticles(float dt);
		void UpdateCameras();
		void UpdateScripts(float dt);

		void ImGuiSceneControl();
		void ImGuiScene();
		void ImGuiEntities();
		void ImGuiAssets();
		void ImGuiCameras();
		void ImGuiInstrumentation();

	};

}

