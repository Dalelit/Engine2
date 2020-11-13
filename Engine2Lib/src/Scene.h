#pragma once
#include "pch.h"
#include "Events.h"
#include "ConstantBuffer.h"
#include "ECS.h"
#include "Entity.h"
#include "GizmoRender.h"
#include "SceneHierarchy.h"
#include "Skybox.h"

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
		void OnRender();
		void OnApplicationEvent(Engine2::ApplicationEvent& event);

		Entity CreateEntity() { return hierarchy.CreateEntity(); }
		Entity GetEntity(EngineECS::EntityId_t id) { return hierarchy.GetEntity(id); }

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

		friend SceneSerialisation;

	protected:
		SceneHierarchy hierarchy;
		Skybox skybox;

		GizmoRender gizmoRender;

		void UpdateVSSceneConstBuffer();
		void UpdatePSSceneConstBuffer();

		void RenderMeshes();
		void RenderParticles();
		void RenderGizmos();
		void RenderOutlines();

		void UpdatePhysics(float dt);
		void UpdateTransformMatrix();
		void UpdateParticles(float dt);

		void ImGuiScene();
		void ImGuiEntities();
		void ImGuiAssets();

	};

}

