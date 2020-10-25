#pragma once
#include "pch.h"
#include "ConstantBuffer.h"
#include "ECS.h"
#include "Entity.h"
#include "GizmoRender.h"
#include "SceneHierarchy.h"

namespace Engine2
{
	// Used for scene level resources that are commonly used and bound once per frame.
	class Scene
	{
	public:
		Scene();
		~Scene() = default;

		void OnUpdate(float dt);
		void OnRender();

		Entity CreateEntity() { return hierarchy.CreateEntity(); }

		void OnImgui();

		struct VSSceneData
		{
			DirectX::XMMATRIX cameraTransform;
		};
		VSConstantBuffer<VSSceneData> vsConstBuffer;

		struct PSSceneData
		{
			DirectX::XMVECTOR CameraPosition;
			DirectX::XMVECTOR ambientLight;
			DirectX::XMVECTOR pointLightPosition;
			DirectX::XMVECTOR pointLightColor;
		};
		PSConstantBuffer<PSSceneData> psConstBuffer;

	protected:
		SceneHierarchy hierarchy;

		GizmoRender gizmoRender;

		void UpdateVSSceneConstBuffer();
		void UpdatePSSceneConstBuffer();

		void RenderMeshes();
		void RenderParticles();
		void RenderGizmos();

		void UpdatePhysics(float dt);
		void UpdateParticles(float dt);

		void ImGuiScene();
		void ImGuiEntities();
		void ImGuiAssets();
	};

}

