#include "pch.h"
#include "Particles.h"

using namespace DirectX;

namespace Engine2
{
	ParticleEmitter::ParticleEmitter(size_t maxParticleCount)
	{
		meshNames.emplace_back("Equilateral Triangle");
		meshNames.emplace_back("Square");

		pixelShaderNames.emplace_back("Solid");
		pixelShaderNames.emplace_back("Circle");
		pixelShaderNames.emplace_back("Test");

		SetMaxParticles(maxParticleCount);

		SetMeshAndVertexShader(meshNames[1]);
		SetPixelShader(pixelShaderNames[1]);
	}

	void ParticleEmitter::OnUpdate(float dt)
	{
		if (freeze) return;

		velocityStartVar = velocityStartMax - velocityStartMin;
		rotationSpeedStartVar = rotationSpeedStartMax - rotationSpeedStartMin;
		colorStartVar = colorStartMax - colorStartMin;
		colorEndVar = colorEndMax - colorEndMin;

		timeSinceLastEmit += dt;

		UINT numberToEmit = (UINT)floorf(timeSinceLastEmit * rate); // given the elapsed time, how many need to be emitted

		if (activeCount + numberToEmit > maxParticles) // cannot emit more than the max
		{
			numberToEmit = (UINT)maxParticles - activeCount; // only emit up to the max particles
		}

		if (numberToEmit > 0) timeSinceLastEmit -= (float)numberToEmit / rate; // accumulating any remaining time if we are emitting something

		UINT numberToUpdate = activeCount + numberToEmit; // we need to process the number of active particle plus the new ones

		E2_ASSERT(numberToUpdate <= particles.size(), "Number of particles exceeded the buffer");

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
			DXDevice::Get().SetNoFaceCullingRenderState();
			DXDevice::Get().SetAlphaBlendState();
			pVS->Bind();
			pPS->Bind();

			//DXDevice::UpdateBuffer(instanceBuffer.GetRawPointer(), instances, instanceCount);
			VB.UpdateInstanceBuffer(instances, instanceCount);
			VB.Bind();
			VB.Draw(instanceCount);
			DXDevice::Get().SetDefaultRenderState();
			DXDevice::Get().SetDefaultBlendState();
		}
	}

	void ParticleEmitter::OnImgui()
	{
		if (ImGui::CollapsingHeader("Particle emitter"))
		{
			ImGui::Checkbox("Freeze", &freeze);
			ImGui::DragFloat3("Position", position.m128_f32, 0.01f);
			
			int maxP = (int)maxParticles;
			if (ImGui::DragInt("Max Particles", &maxP, 1.0f, 0, 100000)) { SetMaxParticles(maxP); }
			
			ImGui::DragFloat("Rate", &rate, 0.1f, 0.01f);
			ImGui::DragFloat3("Force", force.m128_f32, 0.01f);
			if (ImGui::BeginCombo("Mesh", currentMesh.c_str()))
			{
				for (size_t i = 0; i < meshNames.size(); i++)
				{
					bool isSelected = (meshNames[i] == currentMesh);
					if (ImGui::Selectable(meshNames[i].c_str(), isSelected)) SetMeshAndVertexShader(meshNames[i]);
					if (isSelected) ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}
			if (ImGui::BeginCombo("Pixel shader", currentPixelShader.c_str()))
			{
				for (size_t i = 0; i < pixelShaderNames.size(); i++)
				{
					bool isSelected = (pixelShaderNames[i] == currentPixelShader);
					if (ImGui::Selectable(pixelShaderNames[i].c_str(), isSelected)) SetPixelShader(pixelShaderNames[i]);
					if (isSelected) ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}
			ImGui::Text("Emit start params");
			ImGui::DragFloat("Lifespan", &lifeSpan, 0.1f, 0.01f);
			ImGui::DragFloat3("Vel min", velocityStartMin.m128_f32, 0.01f);
			ImGui::DragFloat3("Vel max", velocityStartMax.m128_f32, 0.01f);
			ImGui::DragFloat3("Rot speed min", rotationSpeedStartMin.m128_f32, 0.01f);
			ImGui::DragFloat3("Rot speed max", rotationSpeedStartMax.m128_f32, 0.01f);
			ImGui::DragFloat3("Scale start", scaleStart.m128_f32, 0.01f);
			ImGui::DragFloat3("Scale end", scaleEnd.m128_f32, 0.01f);
			ImGui::ColorEdit4("Col start min", colorStartMin.m128_f32);
			ImGui::ColorEdit4("Col start max", colorStartMax.m128_f32);
			ImGui::ColorEdit4("Col end min", colorEndMin.m128_f32);
			ImGui::ColorEdit4("Col end max", colorEndMax.m128_f32);
			ImGui::Text("Stats");
			ImGui::Text("Active particles %i", activeCount);
			ImGui::Text("Instances %i", instanceCount);
			ImGui::Text("Buffer index %i", bufferIndex);
		}
	}

	void ParticleEmitter::CreateParticle(Particle* pParticle)
	{
		pParticle->life = lifeSpan;
		pParticle->position = position;
		pParticle->velocity = velocityStartMin + velocityStartVar * rng.NextXMVECTORXYZ0();
		pParticle->scale = scaleStart;
		pParticle->scaleDelta = (scaleEnd - scaleStart) / pParticle->life;
		pParticle->rotation = rng.NextXMVECTORXYZ0();
		pParticle->rotationSpeed = rotationSpeedStartMin + rotationSpeedStartVar * rng.NextXMVECTORXYZ0();
		pParticle->color = colorStartMin + colorStartVar * rng.NextXMVECTORXYZ1();
		pParticle->colorDelta = ((colorEndMin + colorEndVar * rng.NextXMVECTORXYZ1()) - pParticle->color) / pParticle->life;
	}

	void ParticleEmitter::UpdateParticle(Particle* pParticle, float dt)
	{
		pParticle->life -= dt;
		pParticle->position += pParticle->velocity * dt;
		pParticle->velocity += force * dt;
		pParticle->rotation += pParticle->rotationSpeed * dt;
		pParticle->scale += pParticle->scaleDelta * dt;
		pParticle->color += pParticle->colorDelta * dt;
	}

	void ParticleEmitter::InstanceParticle(Particle* pParticle, InstanceInfo* pInstance)
	{
		pInstance->transform = XMMatrixTranspose(XMMatrixScalingFromVector(pParticle->scale) * XMMatrixRotationRollPitchYawFromVector(pParticle->rotation) * XMMatrixTranslationFromVector(pParticle->position));
		pInstance->color = pParticle->color;
	}

	void ParticleEmitter::SetMaxParticles(size_t maxParticleCount)
	{
		maxParticles = maxParticleCount;
		particles.resize(maxParticles);
		instances.resize(maxParticles);

		VB.SetInstances<InstanceInfo>(maxParticles);
	}

	void ParticleEmitter::SetMeshAndVertexShader(const std::string& meshName)
	{
		struct Vertex {
			XMFLOAT3 position;
			XMFLOAT2 uv;
		};


		std::vector<D3D11_INPUT_ELEMENT_DESC> vsLayout = {
			{"Position", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"UV"      , 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT   , 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"InstanceTransform", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{"InstanceTransform", 1, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{"InstanceTransform", 2, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{"InstanceTransform", 3, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{"InstanceColor"    , 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA, 1},
		};

		std::vector<Vertex> verticies;
		std::vector<unsigned int> indicies;

		if (meshName == "Equilateral Triangle")
		{
			// equilater triangle, sides size 1.0
			// sqrt (1 - sqr(0.5)) / 2 = 0.4330127018922193
			verticies = {
				{ { -0.5f, -0.4330127018922193f, 0.0f}, {0.0f, 0.0f} },
				{ {  0.0f,  0.4330127018922193f, 0.0f}, {0.0f, 1.0f} },
				{ {  0.5f, -0.4330127018922193f, 0.0f}, {1.0f, 0.0f} },
			};

			indicies = { 0, 1, 2 };
		}
		else if (meshName == "Square")
		{
			verticies = {
				{ { -0.5f, -0.5f, 0.0f}, {0.0f, 0.0f} },
				{ { -0.5f,  0.5f, 0.0f}, {0.0f, 1.0f} },
				{ {  0.5f,  0.5f, 0.0f}, {1.0f, 1.0f} },
				{ {  0.5f, -0.5f, 0.0f}, {1.0f, 0.0f} },
			};

			indicies = { 0, 1, 2, 0, 2, 3 };
		}
		else { E2_ASSERT(false, "Unknwon mesh name for partcile system"); }

		currentMesh = meshName;

		;
		VB.Initialise<Vertex>(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
							  verticies, indicies);
		VB.SetInstances<InstanceInfo>(maxParticles);

		//pVS = std::make_shared<VertexShaderDynamic>(Config::directories["ShaderSourceDir"] + "ParticleVS.hlsl", vsLayout);
		pVS = VertexShader::CreateFromCompiledFile(Config::directories["ShaderCompiledDir"] + "ParticleVS.cso", vsLayout);
	}

	void ParticleEmitter::SetPixelShader(const std::string& shaderName)
	{
		if (shaderName.empty() || shaderName == "Solid") // default
		{
			pPS = PixelShader::CreateFromCompiledFile(Config::directories["ShaderCompiledDir"] + "ParticlePS.cso");
		}
		else if (shaderName == "Circle")
		{
			pPS = PixelShader::CreateFromCompiledFile(Config::directories["ShaderCompiledDir"] + "ParticlePSCircle.cso");
		}
		else if (shaderName == "Test")
		{
			// to do: this is in the lib, but uses a shader file in the app directories...
			pPS = std::make_shared<PixelShaderDynamic>(Config::directories["ShaderSourceDir"] + "ParticlePSTest.hlsl");
		}
		else { E2_ASSERT(false, "Unknwon pixel shader name for partcile system"); }

		currentPixelShader = shaderName;
	}
}
