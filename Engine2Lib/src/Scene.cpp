#include "pch.h"
#include "Scene.h"
#include "Camera.h"
#include "Engine2.h"
#include "Components.h"
#include "MeshRenderer.h"
#include "RigidBody.h"
#include "Particles.h"
#include "Lights.h"
#include "OffscreenOutliner.h"
#include "VertexLayout.h"
#include "MeshLoader.h"
#include "TextureLoader.h"
#include "CameraComponent.h"

using namespace EngineECS;

namespace Engine2
{
	Scene::Scene() : vsConstBuffer(0), psConstBuffer(0)
	{
	}

	void Scene::OnUpdate(float dt)
	{
		UpdatePhysics(dt);
		UpdateTransformMatrix();
		UpdateParticles(dt);
	}

	void Scene::OnRender(Camera& camera)
	{
		RenderImage(camera);
		CamerasRender();
	}

	void Scene::RenderImage(Camera& camera, bool showGizmos)
	{
		UpdateVSSceneConstBuffer(camera);
		UpdatePSSceneConstBuffer(camera);

		RenderMeshes();
		RenderParticles();
		if (skybox.IsActive()) skybox.BindAndDraw();
		RenderOutlines();
		if (gizmoEnabled && showGizmos) RenderGizmos();
	}

	// update any components that need to know when an event has happened
	void Scene::OnApplicationEvent(Engine2::ApplicationEvent& event)
	{
		Coordinator& coordinator = hierarchy.GetECSCoordinator();

		if (event.GetType() == Engine2::EventType::WindowResize)
		{
			for (auto& c : coordinator.GetComponents<OffscreenOutliner>()) c.Reconfigure();
		}
	}

	void Scene::UpdateVSSceneConstBuffer(Camera& camera)
	{
		camera.LoadViewProjectionMatrixT(vsConstBuffer.data.cameraTransform);
		vsConstBuffer.Bind();
	}

	void Scene::UpdatePSSceneConstBuffer(Camera& camera)
	{
		psConstBuffer.data.CameraPosition = camera.GetPosition();

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
		View<Gizmo, TransformMatrix> entities(coordinator);
		gizmoRender.NewFrame();
		for (auto e : entities)
		{
			const auto& gizmo = coordinator.GetComponent<Gizmo>(e);
			const auto& trans = coordinator.GetComponent<TransformMatrix>(e)->transformMatrix;

			switch (gizmo->type)
			{
			case Gizmo::Types::Axis:   gizmoRender.DrawAxis(trans); break;
			case Gizmo::Types::Cube:   gizmoRender.DrawCube(trans); break;
			case Gizmo::Types::Sphere: gizmoRender.DrawSphere(trans); break;
			case Gizmo::Types::Camera: gizmoRender.DrawCamera(trans); break;
			}
		}
		gizmoRender.Render();
	}

	void Scene::RenderOutlines()
	{
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

	void Scene::CamerasRender()
	{
		Coordinator& coordinator = hierarchy.GetECSCoordinator();
		View<CameraComponent, Transform> entities(coordinator);
		for (auto e : entities)
		{
			auto* cc = coordinator.GetComponent<CameraComponent>(e);
			auto* tr = coordinator.GetComponent<Transform>(e);

			cc->GetCamera().Update(); // to do: not real good that this has to be called here.
			cc->Clear();
			cc->SetAsTarget();
			RenderImage(cc->GetCamera(), false);

			cc->ShowSubDisplay();
		}
		DXDevice::Get().SetBackBufferAsRenderTarget();
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

	void Scene::UpdateTransformMatrix()
	{
		hierarchy.UpdateTransformMatrix();
	}

	void Scene::UpdateParticles(float dt)
	{
		Coordinator& coordinator = hierarchy.GetECSCoordinator();
		View<ParticleEmitter, TransformMatrix> entities(coordinator);
		for (auto e : entities)
		{
			auto* emitter = coordinator.GetComponent<ParticleEmitter>(e);
			emitter->SetTransform(coordinator.GetComponent<TransformMatrix>(e)->MatrixTransposed()); // note: transposed as the matrix is ready for GPU
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
			hierarchy.OnImgui();
			ImGui::Separator();
			ImGui::Checkbox("Show Gizmos", &gizmoEnabled);
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
			}
		}
		ImGui::End();
	}

	void Scene::ImGuiAssets()
	{
		static bool open = true;
		if (ImGui::Begin("Assets", &open))
		{
			if (ImGui::TreeNode("Meshes"))
			{
				Mesh::Assets.OnImGui();
				ImGui::TreePop();
			}

			if (ImGui::TreeNode("Materials"))
			{
				Material::Materials.OnImGui();
				ImGui::TreePop();
			}

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

	bool Scene::LoadModel(const std::string& sourceFilename)
	{
		bool result = false;

		auto loadedModel = AssetLoaders::ObjLoader::Load(sourceFilename);

		auto assets = MeshAssetLoader::CreateMeshAssetPositionNormalColor(*loadedModel);
		if (assets.size() > 0) result = true;

		if (loadedModel->textureCoords.size() > 0)
		{
			assets = MeshAssetLoader::CreateMeshAssetPositionNormalTexture(*loadedModel);
			if (assets.size() > 0) result = true;
		}

		return result;
	}
}