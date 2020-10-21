#include "pch.h"
#include "Scene.h"
#include "Camera.h"
#include "Engine2.h"
#include "Components.h"
#include "MeshRenderer.h"
#include "RigidBody.h"
#include "Particles.h"

using namespace EngineECS;

namespace Engine2
{
	Scene::Scene() : vsConstBuffer(0), psConstBuffer(0), coordinator(100)
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

	Entity Scene::CreateEntity()
	{
		EntityId_t id = coordinator.CreateEntity();
		coordinator.AddComponent<EntityInfo>(id)->tag = std::to_string(id);
		coordinator.AddComponent<Transform>(id);
		return Entity(id, coordinator);
	}

	void Scene::OnImgui()
	{
		if (ImGui::TreeNode("Scene"))
		{
			if (ImGui::CollapsingHeader("Lights"))
			{
				ImGui::ColorEdit4("Ambient", psConstBuffer.data.ambientLight.m128_f32);

				for (auto& l : pointLights) l.OnImgui();
			}
			ImGui::TreePop();
		}
		ImGui::Separator();
		if (ImGui::TreeNodeEx("Entities", ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_DefaultOpen))
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
			
			if (ImGui::Button("Add Entity"))
			{
				CreateEntity();
			}
			ImGui::Separator();
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

			ImGui::TreePop();
		}
		ImGui::Separator();
		if (ImGui::TreeNode("Assets"))
		{
			Mesh::Assets.OnImGui();
			Material::Assets.OnImGui();
			ImGui::TreePop();
		}
	}

	void Scene::UpdateVSSceneConstBuffer()
	{
		Engine::GetActiveCamera().LoadViewProjectionMatrixT(vsConstBuffer.data.cameraTransform);
		vsConstBuffer.Bind();
	}

	void Scene::UpdatePSSceneConstBuffer()
	{
		psConstBuffer.data.CameraPosition = Engine::GetActiveCamera().GetPosition();
		if (pointLights.size() > 0)
		{
			psConstBuffer.data.pointLightPosition = pointLights[0].GetPosition();
			psConstBuffer.data.pointLightColor = pointLights[0].GetColor();
		}
		psConstBuffer.Bind();
	}

	void Scene::RenderMeshes()
	{
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
		View<ParticleEmitter, Transform> entities(coordinator);
		for (auto e : entities)
		{
			const auto& emitter = coordinator.GetComponent<ParticleEmitter>(e);
			emitter->OnRender();
		}
	}

	void Scene::RenderGizmos()
	{
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
		View<ParticleEmitter, Transform> entities(coordinator);
		for (auto e : entities)
		{
			auto* emitter = coordinator.GetComponent<ParticleEmitter>(e);
			emitter->SetTransform( coordinator.GetComponent<Transform>(e)->GetTransformTranspose() );
			emitter->OnUpdate(dt);
		}
	}
}