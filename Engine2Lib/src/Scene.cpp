#include "pch.h"
#include "Scene.h"
#include "Camera.h"
#include "Engine2.h"
#include "Components.h"

using namespace EngineECS;

namespace Engine2
{
	Scene::Scene() : vsConstBuffer(0), psConstBuffer(0), coordinator(100)
	{
	}

	void Scene::OnRender()
	{
		UpdateVSConstBuffer();
		UpdatePSConstBuffer();

		// draw mesh entities
		{
			View<Mesh, Transform> entities(coordinator);
			for (auto e : entities)
			{
				const auto& mesh = coordinator.GetComponent<Mesh>(e);

				if (mesh->IsValid())
				{
					mesh->vertexShaderCB->data = *coordinator.GetComponent<Transform>(e);
					mesh->vertexShaderCB->UpdateBuffer();
					mesh->vertexShaderCB->Bind();

					mesh->vertexShader->Bind();
					mesh->pixelShader->Bind();

					mesh->drawable->BindAndDraw();
				}
			}
		}

		// draw gizmos
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
			}
			
			if (ImGui::Button("Add Entity"))
			{
				CreateEntity();
			}

			ImGui::TreePop();
		}
	}

	void Scene::UpdateVSConstBuffer()
	{
		Engine::GetActiveCamera().LoadViewProjectionMatrixT(vsConstBuffer.data.cameraTransform);
		vsConstBuffer.Bind();
	}

	void Scene::UpdatePSConstBuffer()
	{
		psConstBuffer.data.CameraPosition = Engine::GetActiveCamera().GetPosition();
		if (pointLights.size() > 0)
		{
			psConstBuffer.data.pointLightPosition = pointLights[0].GetPosition();
			psConstBuffer.data.pointLightColor = pointLights[0].GetColor();
		}
		psConstBuffer.Bind();
	}
}