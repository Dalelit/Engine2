#include "pch.h"
#include "Particles.h"
#include "UtilMath.h"

using namespace DirectX;

namespace Engine2
{
	void ParticleEmitter::OnUpdate(float dt)
	{
		timerOnUpdate.Set();

		if (freeze) return;

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

		timerOnUpdate.Tick();
	}

	void ParticleEmitter::OnRender()
	{
		timerOnRender.Set();

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

		timerOnRender.Tick();
	}

	void ParticleEmitter::OnImgui()
	{
		ImGui::Checkbox("Freeze", &freeze);
		ImGui::DragFloat3("Position", position.m128_f32, 0.01f);
			
		int maxP = (int)maxParticles;
		if (ImGui::DragInt("Max Particles", &maxP, 1.0f, 0, 100000)) { if (maxP > 0) SetMaxParticles(maxP); }
			
		if (ImGui::DragFloat("Rate", &rate, 0.1f)) { if (rate < 0.0f) rate = 0.0f; }
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
		if (ImGui::DragFloat("Lifespan", &lifeSpan, 0.1f)) { if (lifeSpan < 0.0f) lifeSpan = 0.0f; }
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
		ImGui::Text("OnUpdate %fms", timerOnUpdate.Average());
		ImGui::Text("OnRender %fms", timerOnRender.Average());
		ImGui::Text("Active particles %i", activeCount);
		ImGui::Text("Instances %i", instanceCount);
		ImGui::Text("Buffer index %i", bufferIndex);
	}

	void ParticleEmitter::DefaultInitialisation(size_t maxParticles)
	{
		SetMaxParticles(maxParticles);
		SetMeshAndVertexShader(meshNames[1]);
		SetPixelShader(pixelShaderNames[1]);
	}

	void ParticleEmitter::CreateParticle(Particle* pParticle)
	{
		pParticle->life = lifeSpan;
		pParticle->position = emitLocation;
		pParticle->velocity = velocityStartMin + ((velocityStartMax - velocityStartMin) * rng.NextXMVECTORXYZ0());
		pParticle->scale = scaleStart;
		pParticle->scaleDelta = (scaleEnd - scaleStart) / pParticle->life;
		pParticle->rotation = rng.NextXMVECTORXYZ0();
		pParticle->rotationSpeed = rotationSpeedStartMin + ((rotationSpeedStartMax - rotationSpeedStartMin) * rng.NextXMVECTORXYZ0());
		pParticle->color = colorStartMin + ((colorStartMax - colorStartMin) * rng.NextXMVECTORXYZ1());
		pParticle->colorDelta = ((colorEndMin + ((colorEndMax - colorEndMin) * rng.NextXMVECTORXYZ1())) - pParticle->color) / pParticle->life;
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
		pInstance->transform = XMMatrixTranspose(Math::TransformMatrixEuler(pParticle->position, pParticle->scale, pParticle->rotation));
		pInstance->color = pParticle->color;
	}

	void ParticleEmitter::SetMaxParticles(size_t maxParticleCount)
	{
		E2_ASSERT(maxParticleCount > 0, "Do not set max particle count below 1");

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
			pPS = std::make_shared<PixelShaderFile>(Config::directories["ShaderSourceDir"] + "ParticlePSTest.hlsl");
		}
		else { E2_ASSERT(false, "Unknwon pixel shader name for partcile system"); }

		currentPixelShader = shaderName;
	}

	void ParticleEmitter::Serialise(Serialisation::INode& node)
	{
		node.Attribute("active", active);
		node.Attribute("freeze", freeze);
		node.Attribute("position", position);
		node.Attribute("emitLocation", emitLocation);
		node.Attribute("velocityStartMin", velocityStartMin);
		node.Attribute("velocityStartMax", velocityStartMax);
		node.Attribute("force", force);
		node.Attribute("rotationSpeedStartMin", rotationSpeedStartMin);
		node.Attribute("rotationSpeedStartMax", rotationSpeedStartMax);
		node.Attribute("colorStartMin", colorStartMin);
		node.Attribute("colorStartMax", colorStartMax);
		node.Attribute("colorEndMin", colorEndMin);
		node.Attribute("colorEndMax", colorEndMax);
		node.Attribute("scaleStart", scaleStart);
		node.Attribute("scaleEnd", scaleEnd);

		if (node.Saving())
		{
			node.Attribute("rate", rate);
			node.Attribute("maxParticles", maxParticles);
		}
		else
		{
			float _r;
			node.Attribute("rate", _r);
			SetRate(_r);

			size_t _mp;
			node.Attribute("maxParticles", _mp);
			SetMaxParticles(_mp);
		}
	}
}
