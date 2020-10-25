#include "pch.h"
#include "Scene.h"
#include "Camera.h"
#include "Engine2.h"
#include "Components.h"
#include "MeshRenderer.h"
#include "RigidBody.h"
#include "Particles.h"
#include "Lights.h"

using namespace EngineECS;

namespace Engine2
{
	Scene::Scene() : vsConstBuffer(0), psConstBuffer(0)
	{
	}

	void Scene::OnUpdate(float dt)
	{
		UpdatePhysics(dt);
		UpdateParticles(dt);
	}

	void Scene::OnRender()
	{
		UpdateVSSceneConstBuffer();
		UpdatePSSceneConstBuffer();

		RenderMeshes();
		RenderParticles();
		RenderGizmos();
	}

	void Scene::UpdateVSSceneConstBuffer()
	{
		Engine::GetActiveCamera().LoadViewProjectionMatrixT(vsConstBuffer.data.cameraTransform);
		vsConstBuffer.Bind();
	}

	void Scene::UpdatePSSceneConstBuffer()
	{
		psConstBuffer.data.CameraPosition = Engine::GetActiveCamera().GetPosition();

		Coordinator& coordinator = hierarchy.GetECSCoordinator();
		auto& pointLights = coordinator.GetComponents<PointLight>();

		if (pointLights.Count() > 0)
		{
			
			auto position = coordinator.GetComponent<Transform>(pointLights.GetEntity(0))->GetTranslation();
			psConstBuffer.data.pointLightPosition = position;
			psConstBuffer.data.pointLightColor = pointLights[0].GetColor();
		}
		psConstBuffer.Bind();
	}

	void Scene::RenderMeshes()
	{
		Coordinator& coordinator = hierarchy.GetECSCoordinator();
		View<MeshRenderer, Transform> entities(coordinator);
		for (auto e : entities)
		{
			const auto& mr = coordinator.GetComponent<MeshRenderer>(e);

			if (mr->IsValid())
			{
				mr->material->SetTransform(*coordinator.GetComponent<Transform>(e));
				mr->BindAndDraw();
			}
		}
	}

	void Scene::RenderParticles()
	{
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
		Coordinator& coordinator = hierarchy.GetECSCoordinator();
		View<Gizmo, Transform> entities(coordinator);
		gizmoRender.NewFrame();
		for (auto e : entities)
		{
			const auto& gizmo = coordinator.GetComponent<Gizmo>(e);
			const auto& trans = coordinator.GetComponent<Transform>(e);

			switch (gizmo->type)
			{
			case Gizmo::Types::Axis:   gizmoRender.DrawAxis(trans->transform); break;
			case Gizmo::Types::Cube:   gizmoRender.DrawCube(trans->transform); break;
			case Gizmo::Types::Sphere: gizmoRender.DrawSphere(trans->transform); break;
			case Gizmo::Types::Camera: gizmoRender.DrawCamera(trans->transform); break;
			}
		}
		gizmoRender.Render();
	}

	void Scene::UpdatePhysics(float dt)
	{
		Coordinator& coordinator = hierarchy.GetECSCoordinator();
		View<RigidBody, Transform> entities(coordinator);
		for (auto e : entities)
		{
			auto* rb = coordinator.GetComponent<RigidBody>(e);
			auto* tr = coordinator.GetComponent<Transform>(e);
			rb->OnUpdate(dt, tr);
		}
	}

	void Scene::UpdateParticles(float dt)
	{
		Coordinator& coordinator = hierarchy.GetECSCoordinator();
		View<ParticleEmitter, Transform> entities(coordinator);
		for (auto e : entities)
		{
			auto* emitter = coordinator.GetComponent<ParticleEmitter>(e);
			emitter->SetTransform( coordinator.GetComponent<Transform>(e)->GetTransformTranspose() );
			emitter->OnUpdate(dt);
		}
	}

	void Scene::OnImgui()
	{
		ImGuiScene();
		ImGuiEntities();
		ImGuiAssets();
	}

	void Scene::ImGuiScene()
	{
		static bool sceneOpen = true;
		if (ImGui::Begin("Scene", &sceneOpen))
		{
			// to do: temp
			ImGui::ColorEdit4("Ambient Light", psConstBuffer.data.ambientLight.m128_f32);

			hierarchy.OnImGui();
			ImGui::End();
		}

		static bool entityOpen = true;
		if (ImGui::Begin("Entity", &entityOpen))
		{
			hierarchy.SelectedEntityOnImGui();
			ImGui::End();
		}
	}

	void Scene::ImGuiEntities()
	{
		static bool open = true;
		if (ImGui::Begin("Entities", &open))
		{
			Coordinator& coordinator = hierarchy.GetECSCoordinator();

			if (ImGui::TreeNodeEx("Stats", ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_DefaultOpen))
			{
				ImGui::Text("Entities: %i/%i", coordinator.GetEntityCount(), coordinator.GetMaxEntities());
				ImGui::Text("Components:");
				for (uint32_t i = 0; i < coordinator.GetComponentCount(); i++)
				{
					auto* pStore = coordinator.GetComponentStore(i);
					if (pStore)
					{
						ImGui::Text("%i %s: %i/%i", i, pStore->GetName().c_str(), pStore->Count(), pStore->Capacity());
					}
				}
				ImGui::TreePop();
			}

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

			ImGui::End();
		}
	}

	void Scene::ImGuiAssets()
	{
		static bool open = true;
		if (ImGui::Begin("Assets", &open))
		{
			Mesh::Assets.OnImGui();
			Material::Assets.OnImGui();
			ImGui::End();
		}
	}
}