#include "pch.h"
#include "Particles.h"

using namespace DirectX;

namespace Engine2
{
	ParticleEmitter::ParticleEmitter()
	{
		particles.resize(maxParticles);
		instances.resize(maxParticles);

		SetMeshAndVertexShader();
		SetPixelShader();
	}

	void ParticleEmitter::OnUpdate(float dt)
	{
		velocityStartVar = velocityStartMax - velocityStartMin;
		colorStartVar = colorStartMax - colorStartMin;

		timeSinceLastEmit += dt;

		UINT numberToEmit = (UINT)floorf(timeSinceLastEmit * rate); // given the elapsed time, how many need to be emitted
		timeSinceLastEmit -= (float)numberToEmit / rate;            // accumulating any remaining time

		UINT numberToUpdate = activeCount + numberToEmit; // we need to process the number of active particle plus the new ones

		E2_ASSERT(numberToUpdate < particles.size(), "Number of particles exceeded the buffer");

		Particle* pParticle = particles.data();
		InstanceInfo* pInstance = instances.data();
		
		while (numberToUpdate > 0)
		{
			if (pParticle->life > 0.0f)
			{
				numberToUpdate--; // will update it

				UpdateParticle(pParticle, dt);
				if (pParticle->life > 0.0f)
				{
					InstanceParticle(pParticle, pInstance);
					pInstance++;
				}
				else
				{
					activeCount--; // was alive, just died
				}
			}
			else if (numberToEmit > 0)
			{
				numberToUpdate--; // will update it

				CreateParticle(pParticle);
				InstanceParticle(pParticle, pInstance);
				pInstance++;
				numberToEmit--;
				activeCount++;
			}

			pParticle++;
		}

		// Just to help understand how the buffers are being used. Displayed in imgui.
		instanceCount = (UINT)(pInstance - instances.data());
		bufferIndex = (UINT)(pParticle - particles.data());
	}

	void ParticleEmitter::OnRender()
	{
		if (instanceCount > 0)
		{
			pVS->Bind();
			pPS->Bind();

			DXDevice::UpdateBuffer(instanceBuffer, instances, instanceCount);
			pVB->Bind();
			pVB->Draw(instanceCount);
		}
	}

	void ParticleEmitter::OnImgui()
	{
		if (ImGui::CollapsingHeader("Particle emitter"))
		{
			ImGui::DragFloat3("Position", position.m128_f32, 0.01f);
			ImGui::DragFloat("Rate", &rate, 0.1f, 0.01f);
			ImGui::DragFloat3("Start velocity min", velocityStartMin.m128_f32, 0.01f);
			ImGui::DragFloat3("Start velocity max", velocityStartMax.m128_f32, 0.01f);
			ImGui::DragFloat3("Force", force.m128_f32, 0.01f);
			ImGui::ColorEdit3("Start color min", colorStartMin.m128_f32);
			ImGui::ColorEdit3("Start color max", colorStartMax.m128_f32);
			ImGui::Text("Active particles %i", activeCount);
			ImGui::Text("Instances %i", instanceCount);
			ImGui::Text("Buffer index %i", bufferIndex);
		}
	}

	void ParticleEmitter::CreateParticle(Particle* pParticle)
	{
		pParticle->position = position;
		pParticle->color = colorStartMin + colorStartVar * rng.NextXMVECTORXYZ1(); // to do: sort out alpha
		pParticle->life = 5.0f;
		pParticle->velocity = velocityStartMin + velocityStartVar * rng.NextXMVECTORXYZ0();
		pParticle->scale = { 0.1f, 0.1f, 0.1f, 0.0f };
	}

	void ParticleEmitter::UpdateParticle(Particle* pParticle, float dt)
	{
		pParticle->life -= dt;
		pParticle->position += pParticle->velocity * dt;
		pParticle->velocity += force * dt;
	}

	void ParticleEmitter::InstanceParticle(Particle* pParticle, InstanceInfo* pInstance)
	{
		pInstance->transform = XMMatrixTranspose(XMMatrixScalingFromVector(pParticle->scale) * XMMatrixTranslationFromVector(pParticle->position));
		pInstance->color = pParticle->color;
	}

	void ParticleEmitter::SetMeshAndVertexShader()
	{
		struct Vertex {
			XMFLOAT3 position;
		};


		std::vector<D3D11_INPUT_ELEMENT_DESC> vsLayout = {
			{"Position", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"InstanceTransform", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{"InstanceTransform", 1, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{"InstanceTransform", 2, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{"InstanceTransform", 3, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{"InstanceColor"    , 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA, 1},
		};

		// equilater triangle, sides size 1.0
		// sqrt (1 - sqr(0.5)) / 2 = 0.4330127018922193
		std::vector<Vertex> verticies = {
			{ { -0.5f, -0.4330127018922193f, 0.0f} },
			{ {  0.0f,  0.4330127018922193f, 0.0f} },
			{ {  0.5f, -0.4330127018922193f, 0.0f} },
		};

		std::vector<unsigned int> indicies = {0, 1, 2};

		auto vb = std::make_shared<TriangleListIndexInstanced<Vertex, InstanceInfo>>(verticies, indicies);
		instanceBuffer = vb->AddInstances(instances, true);
		pVB = vb;
		pVS = std::make_shared<VertexShaderDynamic>(Config::directories["ShaderSourceDir"] + "ParticleVS.hlsl", vsLayout);
	}

	void ParticleEmitter::SetPixelShader()
	{
		pPS = std::make_shared<PixelShaderDynamic>(Config::directories["ShaderSourceDir"] + "ParticlePS.hlsl");
	}
}
