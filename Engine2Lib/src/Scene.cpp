#include "pch.h"
#include "Scene.h"
#include "Camera.h"
#include "Engine2.h"
#include "Components.h"
#include "EntityInfo.h"
#include "Transform.h"
#include "MeshRenderer.h"
#include "RigidBody.h"
#include "Collider.h"
#include "Particles.h"
#include "Lights.h"
#include "OffscreenOutliner.h"
#include "VertexLayout.h"
#include "TextureLoader.h"
#include "ScriptComponent.h"
#include "AssetManager.h"

//#define E2_SCENE_TIMING
#define E2_SCENE_TIMING    auto timer##_COUNTER__ = systemTimers.ScopeTimer(__FUNCTION__);


using namespace EngineECS;

namespace Engine2
{
	using namespace EngineECS;
	using namespace DirectX;

	Scene::Scene() : vsConstBuffer(0), psConstBuffer(0)
	{
		physics.Initialise();
	}

	void Scene::OnUpdate(float dt)
	{
		E2_SCENE_TIMING;

		if (running)
		{
			if (!paused)
			{
				UpdatePhysics(dt);
			}
		}
		else
		{
			UpdateTransformMatrix();
		}
		UpdateParticles(dt);
		UpdateCameras();
		UpdateScripts(dt);
	}

	void Scene::OnRender(EntityId_t cameraEntity)
	{
		E2_SCENE_TIMING;
		ShadowPass(cameraEntity);
		RenderImage(cameraEntity, true);
		CamerasRender();
	}

	void Scene::RenderImage(EntityId_t cameraEntity, bool mainCamera)
	{
		E2_SCENE_TIMING;
		auto pCamera = hierarchy.GetECSCoordinator().GetComponent<Camera>(cameraEntity);
		auto pTransform = hierarchy.GetECSCoordinator().GetComponent<Transform>(cameraEntity);
		UpdateVSSceneConstBuffer(*pCamera, *pTransform);
		UpdatePSSceneConstBuffer(*pCamera, *pTransform);

		RenderMeshes();
		RenderParticles();
		if (skybox.IsActive() && pCamera->IsPerspective()) skybox.BindAndDraw();

		if (mainCamera)
		{
			RenderOutlines();
			if (gizmoEnabled) RenderGizmos();
			if (gizmoCollidersEnabled) RenderColliders();
		}
	}

	void Scene::ShadowPass(EntityId_t viewCameraEntity)
	{
		E2_SCENE_TIMING;
		auto pCamera = hierarchy.GetECSCoordinator().GetComponent<Camera>(viewCameraEntity);
		auto pCameraTransform = hierarchy.GetECSCoordinator().GetComponent<TransformMatrix>(viewCameraEntity);

		sun.ShadowPassStart(WorldCamera(*pCamera, pCameraTransform->GetTransform()));

		//UpdateVSSceneConstBuffer(light.GetCamera(), *pTransform);
		sun.GetCamera().LoadViewProjectionMatrixT(vsConstBuffer.data.cameraTransform);
		vsConstBuffer.Bind();

		//UpdatePSSceneConstBuffer(light.GetCamera(), *pTransform);
		DXDevice::GetContext().PSSetShader(nullptr, nullptr, 0u);

		//RenderMeshes();
		Coordinator& coordinator = hierarchy.GetECSCoordinator();
		View<MeshRenderer, Transform> entities(coordinator);
		for (auto e : entities)
		{
			const auto& mr = coordinator.GetComponent<MeshRenderer>(e);

			if (mr->IsValid())
			{
				mr->material->SetTransform(*coordinator.GetComponent<TransformMatrix>(e));
				mr->ShadowBindAndDraw();
			}
		}

		//RenderParticles();

		sun.ShadowPassEnd();
		sun.BindShadowMap();
	}

	// update any components that need to know when an event has happened
	void Scene::OnApplicationEvent(Engine2::ApplicationEvent& event)
	{
		Coordinator& coordinator = hierarchy.GetECSCoordinator();

		if (event.GetType() == Engine2::EventType::WindowResize)
		{
			for (auto& c : coordinator.GetComponents<Camera>()) if (c.IsAspectRatioLockedToScreen()) c.SetAspectRatio(DXDevice::Get().GetAspectRatio());
			for (auto& c : coordinator.GetComponents<OffscreenOutliner>()) c.Reconfigure();
		}
	}

	void Scene::UpdateVSSceneConstBuffer(Camera& camera, Transform& transform)
	{
		camera.LoadViewProjectionMatrixT(vsConstBuffer.data.cameraTransform);
		vsConstBuffer.Bind();
	}

	void Scene::UpdatePSSceneConstBuffer(Camera& camera, Transform& transform)
	{
		psConstBuffer.data.CameraPosition = transform.position;

		Coordinator& coordinator = hierarchy.GetECSCoordinator();
		auto& pointLights = coordinator.GetComponents<PointLight>();

		if (pointLights.Count() > 0)
		{
			auto position = coordinator.GetComponent<TransformMatrix>(pointLights.GetEntity(0))->GetTranslation();
			psConstBuffer.data.pointLightPosition = position;
			psConstBuffer.data.pointLightColor = pointLights[0].color;
		}
		else
		{
			psConstBuffer.data.pointLightColor = {};
		}

		psConstBuffer.Bind();
	}

	void Scene::RenderMeshes()
	{
		E2_SCENE_TIMING;
		Coordinator& coordinator = hierarchy.GetECSCoordinator();
		View<MeshRenderer, Transform> entities(coordinator);
		for (auto e : entities)
		{
			const auto& mr = coordinator.GetComponent<MeshRenderer>(e);

			if (mr->IsValid())
			{
				mr->material->SetTransform(*coordinator.GetComponent<TransformMatrix>(e));
				mr->BindAndDraw();
			}
		}
	}

	void Scene::RenderParticles()
	{
		E2_SCENE_TIMING;
		Coordinator& coordinator = hierarchy.GetECSCoordinator();
		View<ParticleEmitter, Transform> entities(coordinator);
		for (auto e : entities)
		{
			const auto& emitter = coordinator.GetComponent<ParticleEmitter>(e);
			emitter->OnRender();
		}
	}

	void Scene::RenderGizmos()
	{
		E2_SCENE_TIMING;
		Coordinator& coordinator = hierarchy.GetECSCoordinator();
		View<Gizmo, TransformMatrix> entities(coordinator);
		gizmoRender.NewFrame();
		for (auto e : entities)
		{
			const auto& gizmo = coordinator.GetComponent<Gizmo>(e);
			const auto& trans = coordinator.GetComponent<TransformMatrix>(e)->GetTransform();

			switch (gizmo->type)
			{
			case Gizmo::Types::Axis:   gizmoRender.DrawAxis(trans); break;
			case Gizmo::Types::Cube:   gizmoRender.DrawCube(trans); break;
			case Gizmo::Types::Sphere: gizmoRender.DrawSphere(trans); break;
			case Gizmo::Types::Camera:
				auto pCamera = coordinator.GetComponent<Camera>(e);
				if (pCamera)
					gizmoRender.DrawCamera(trans, pCamera->GetFrustrumPoints());
				break;
			}
		}

		gizmoRender.Render();
	}

	void Scene::RenderOutlines()
	{
		E2_SCENE_TIMING;
		Coordinator& coordinator = hierarchy.GetECSCoordinator();
		View<OffscreenOutliner, MeshRenderer, Transform> entities(coordinator);
		for (auto e : entities)
		{
			// sets the world transform certex constant buffer
			auto mr = coordinator.GetComponent<MeshRenderer>(e);
			if (mr->IsValid())
			{
				mr->material->SetTransform(*coordinator.GetComponent<TransformMatrix>(e));
				mr->material->vertexShaderCB->Bind();

				// binds the vertex buffer
				auto drawable = mr->mesh;
				drawable->Bind();

				auto outliner = coordinator.GetComponent<OffscreenOutliner>(e);
				outliner->Clear();

				outliner->SetForWriteMask();
				drawable->Draw();
				outliner->SetForOutline();
				drawable->Draw();
				outliner->DrawToBackBuffer();
			}
		}
	}

	void Scene::RenderColliders()
	{
		E2_SCENE_TIMING;
		Coordinator& coordinator = hierarchy.GetECSCoordinator();
		View<Collider, Transform, TransformMatrix> entities(coordinator);
		gizmoRender.NewFrame();
		for (auto e : entities)
		{
			const auto& collider = coordinator.GetComponent<Collider>(e);
			const auto& trans = coordinator.GetComponent<Transform>(e);
			const auto& transMatrix = coordinator.GetComponent<TransformMatrix>(e);

			switch (collider->GetType())
			{
			case Collider::ColliderType::sphere:
			{
				float radius = collider->Radius() * Math::GetMaxFloatVector3(trans->scale);
				XMMATRIX m = XMMatrixScaling(radius, radius, radius) * transMatrix->GetRotation() * XMMatrixTranslationFromVector(transMatrix->GetTranslation());
				gizmoRender.DrawSphere(m, GizmoRender::colliderColor);
				break;
			}
			case Collider::ColliderType::box:
			{
				XMVECTOR scale = trans->scale * XMLoadFloat3(&collider->HalfExtents());
				XMMATRIX m = XMMatrixScalingFromVector(scale * 2.0f) * transMatrix->GetRotation() * XMMatrixTranslationFromVector(transMatrix->GetTranslation());
				gizmoRender.DrawCube(m, GizmoRender::colliderColor);
				break;
			}
			}
		}

		gizmoRender.Render();
	}

	void Scene::CamerasRender()
	{
		E2_SCENE_TIMING;
		Coordinator& coordinator = hierarchy.GetECSCoordinator();
		View<Camera, Offscreen> entities(coordinator);
		for (auto e : entities)
		{
			//auto* ca = coordinator.GetComponent<Camera>(e);
			//auto* tr = coordinator.GetComponent<Transform>(e);
			auto* of = coordinator.GetComponent<Offscreen>(e);

			of->Clear();
			of->SetAsTarget();
			RenderImage(e, false);

			of->ShowSubDisplay();
		}
		DXDevice::Get().SetBackBufferAsRenderTarget();
	}

	void Scene::LoadPhysics()
	{
		physics.CreateScene();

		Coordinator& coordinator = hierarchy.GetECSCoordinator();
		View<Transform, RigidBody, Collider> entities(coordinator);
		for (auto e : entities)
		{
			physics.AddEntity(e, *coordinator.GetComponent<Transform>(e), *coordinator.GetComponent<RigidBody>(e), *coordinator.GetComponent<Collider>(e));
		}
	}

	void Scene::ClearPhysics()
	{
		physics.ClearScene();
	}

	void Scene::UpdatePhysics(float dt)
	{
		E2_SCENE_TIMING;

		physics.StepSimulation(dt);
		physics.UpdateTransforms(hierarchy.GetECSCoordinator());
	}

	void Scene::UpdateTransformMatrix()
	{
		E2_SCENE_TIMING;
		hierarchy.UpdateTransformMatrix();
	}

	void Scene::UpdateParticles(float dt)
	{
		E2_SCENE_TIMING;
		Coordinator& coordinator = hierarchy.GetECSCoordinator();
		View<ParticleEmitter, TransformMatrix> entities(coordinator);
		for (auto e : entities)
		{
			auto* emitter = coordinator.GetComponent<ParticleEmitter>(e);
			emitter->SetTransform(coordinator.GetComponent<TransformMatrix>(e)->GetTransform());
			emitter->OnUpdate(dt);
		}
	}

	void Scene::UpdateCameras()
	{
		E2_SCENE_TIMING;
		Coordinator& coordinator = hierarchy.GetECSCoordinator();

		auto& cameras = coordinator.GetComponents<Camera>();
		for (auto iter = cameras.begin(); iter != cameras.end(); ++iter)
		{
			auto t = coordinator.GetComponent<Transform>(iter.EntityId());
			(*iter).Update(t->position, t->rotation);
		}
	}

	void Scene::UpdateScripts(float dt)
	{
		E2_SCENE_TIMING;
		Coordinator& coordinator = hierarchy.GetECSCoordinator();

		auto& scs = coordinator.GetComponents<ScriptComponent>();
		for (auto& sc : scs)
		{
			sc.OnUpdate(dt);
		}
	}

	Entity Scene::CreateSceneCamera(const std::string& name, bool makeMainCamera)
	{
		auto entity = CreateEntity();
		auto camera = entity.AddComponent<Camera>(name);
		camera->SetAspectRatio( DXDevice::Get().GetAspectRatio() );
		entity.GetComponent<EntityInfo>()->tag = name;

		if (makeMainCamera) SetMainSceneCarmera(entity.Id());

		return entity;
	}

	void Scene::Clear()
	{
		hierarchy.Clear();
	}

	void Scene::OnImgui()
	{
		ImGuiSceneControl();
		ImGuiScene();
		ImGuiEntities();
		ImGuiAssets();
		ImGuiCameras();
		ImGuiInstrumentation();
	}

	void Scene::Start()
	{
		running = true;
		paused = false;
		LoadPhysics();
	}

	void Scene::Stop()
	{
		running = false;
		ClearPhysics();
	}

	void Scene::Pause()
	{
		paused = true;
	}

	void Scene::Resume()
	{
		paused = false;
	}

	void Scene::ImGuiSceneControl()
	{
		static bool controlOpen = true;
		if (ImGui::Begin("Run control", &controlOpen))
		{
			if (running)
			{
				if (ImGui::Button("Stop")) Stop();
				ImGui::SameLine();
				if (paused)
				{
					if (ImGui::Button("Resume")) Resume();
				}
				else
				{
					if (ImGui::Button("Paused")) Pause();
				}
			}
			else
			{
				if (ImGui::Button("Start")) Start();
			}
		}
		ImGui::End();
	}

	void Scene::ImGuiScene()
	{
		static bool sceneOpen = true;
		if (ImGui::Begin("Scene", &sceneOpen))
		{
			hierarchy.OnImgui();
			ImGui::Separator();
			ImGui::Checkbox("Show Gizmos", &gizmoEnabled);
			ImGui::Checkbox("Show Collider Gizmos", &gizmoCollidersEnabled);
			if (ImGui::TreeNode("Sun"))
			{
				sun.OnImgui();
				ImGui::TreePop();
			}
			skybox.OnImgui();
		}
		ImGui::End();

		static bool entityOpen = true;
		if (ImGui::Begin("Entity", &entityOpen))
		{
			hierarchy.SelectedEntityOnImgui();
		}
		ImGui::End();
	}

	void Scene::ImGuiEntities()
	{
		static bool open = true;
		if (ImGui::Begin("Entities", &open))
		{
			Coordinator& coordinator = hierarchy.GetECSCoordinator();

			ImGui::Text("Stats:");
			ImGui::Text(" Entities  : %i/%i", coordinator.GetEntityCount(), coordinator.GetMaxEntities());
			ImGui::Text(" Components: %i/%i", coordinator.GetComponentCount(), EngineECS::MAXCOMPONENTS);

			if (ImGui::TreeNodeEx("Components", ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_DefaultOpen))
			{
				for (uint32_t i = 0; i < coordinator.GetComponentCount(); i++)
				{
					auto* pStore = coordinator.GetComponentStore(i);
					if (pStore)
					{
						ImGui::Text(" %i %s: %i/%i", i, pStore->GetName().c_str(), pStore->Count(), pStore->Capacity());
					}
					else
					{
						ImGui::Text(" %i %s:", i, coordinator.GetComponentName(i));
					}
				}
				ImGui::TreePop();
			}

			if (ImGui::TreeNode("Entities"))
			{
				auto& allInfo = coordinator.GetComponents<EntityInfo>(); // Note: All entities when created through scene get an entityInfo component
				for (EngineECS::ComponentIndex_t indx = 0; indx < allInfo.Count(); indx++)
				{
					if (ImGui::TreeNode(allInfo[indx].tag.c_str()))
					{
						Components::OnImgui(allInfo.GetEntity(indx), coordinator);
						ImGui::TreePop();
					}
					ImGui::Separator();
				}
				ImGui::TreePop();
			}
		}
		ImGui::End();
	}

	void Scene::ImGuiAssets()
	{
		static bool open = true;
		if (ImGui::Begin("Assets", &open))
		{
			AssetManager::Manager().OnImgui();
			ImGui::Separator();

			if (ImGui::TreeNode("Vertex Shaders"))
			{
				Material::VertexShaders.OnImGui();
				ImGui::TreePop();
			}

			if (ImGui::TreeNode("Pixel Shaders"))
			{
				Material::PixelShaders.OnImGui();
				ImGui::TreePop();
			}

			if (ImGui::TreeNode("Textures"))
			{
				TextureLoader::Textures.OnImGui();
				ImGui::TreePop();
			}
		}
		ImGui::End();
	}

	void Scene::ImGuiCameras()
	{
		static bool open = true;

		if (ImGui::Begin("Cameras", &open))
		{
			auto& cameras = hierarchy.GetECSCoordinator().GetComponents<Camera>();

			if (ImGui::BeginCombo("", cameras[mainCameraEntity].GetName().c_str()))
			{
				for (auto iter = cameras.begin(); iter != cameras.end(); ++iter)
				{
					bool isSelected = (iter.EntityId() == mainCameraEntity);
					if (ImGui::Selectable((*iter).GetName().c_str(), isSelected)) SetMainSceneCarmera(iter.EntityId());
					if (isSelected) ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}

			for (auto iter = cameras.begin(); iter != cameras.end(); ++iter)
			{
				iter->OnImgui();
			}
		}
		ImGui::End();
	}

	void Scene::ImGuiInstrumentation()
	{
		ImGui::SetNextWindowBgAlpha(0.35f); // Transparent background
		ImVec4 bg = ImGui::GetStyleColorVec4(ImGuiCol_FrameBg); // store the current background color
		ImGui::GetStyle().Colors[ImGuiCol_FrameBg] = { 0,0,0,0 }; // set background to transparent

		static bool open = true;

		if (ImGui::Begin("Scene instrumentation", &open))
		{
			systemTimers.OnImgui();
		}

		ImGui::End();

		ImGui::GetStyle().Colors[ImGuiCol_FrameBg] = bg; // recover the background color
	}
}