#pragma once
#include "pch.h"
#include "VertexBufferInstanced.h"
#include "ConstantBuffer.h"
#include "Shader.h"
#include "Util.h"
#include "Instrumentation.h"

namespace Engine2
{
	struct Particle
	{
	public:
		DirectX::XMVECTOR position;
		DirectX::XMVECTOR velocity;
		DirectX::XMVECTOR color;
		DirectX::XMVECTOR colorDelta;
		DirectX::XMVECTOR scale;
		DirectX::XMVECTOR scaleDelta;
		DirectX::XMVECTOR rotation;
		DirectX::XMVECTOR rotationSpeed;
		float life;
	};

	class ParticleEmitter
	{
	public:
		ParticleEmitter(size_t maxParticleCount = 100);

		void OnUpdate(float dt);
		void OnRender();
		void OnImgui();

		inline bool IsActive() { return active; }
		inline void SetActive(bool makeActive = true) { active = makeActive; }

		inline void SetTransform(DirectX::XMMATRIX transform) { emitLocation = DirectX::XMVector3Transform(position, transform); }

		void SetMaxParticles(size_t maxParticleCount);
		size_t GetMaxParticles() { return maxParticles; }

		void SetRate(float newRate) { rate = newRate; }

		void SetMeshAndVertexShader(const std::string& meshName);
		void SetPixelShader(const std::string& shaderName = "");

	protected:
		bool active = true;
		bool freeze = false;
		DirectX::XMVECTOR position;
		DirectX::XMVECTOR emitLocation;
		float rate = 10.0; // per second
		size_t maxParticles;
		float timeSinceLastEmit = 0.0f;
		std::vector<Particle> particles;
		UINT activeCount = 0;
		UINT bufferIndex; // used to debug how the buffer is used

		struct InstanceInfo {
			DirectX::XMMATRIX transform;
			DirectX::XMVECTOR color;
		};
		std::vector<InstanceInfo> instances;
		UINT instanceCount = 0;

		VertexBufferIndexInstanced VB;
		std::shared_ptr<VertexShader> pVS;
		std::shared_ptr<PixelShader> pPS;

		std::vector<std::string> pixelShaderNames;
		std::string currentPixelShader;
		std::vector<std::string> meshNames;
		std::string currentMesh;

		Util::Random rng;

		Instrumentation::Timer timerOnRender;
		Instrumentation::Timer timerOnUpdate;

		// start parameters
		float lifeSpan = 4.0f;

		DirectX::XMVECTOR velocityStartMin = { -0.5f, 1.0f, -0.5f, 0.0f };
		DirectX::XMVECTOR velocityStartMax = {  0.5f, 2.0f,  0.5f, 0.0f };
		DirectX::XMVECTOR velocityStartVar; // calculated in onupdate
		DirectX::XMVECTOR force = { 0.0f, -0.3f, 0.0f, 0.0f };

		DirectX::XMVECTOR rotationSpeedStartMin = { -1.0f, -1.0f, -1.0f, 0.0f };
		DirectX::XMVECTOR rotationSpeedStartMax = { 1.0f, 1.0f,  1.0f, 0.0f };
		DirectX::XMVECTOR rotationSpeedStartVar; // calculated in onupdate

		DirectX::XMVECTOR colorStartMin = { 1.0f, 0.0f, 0.0f, 1.0f };
		DirectX::XMVECTOR colorStartMax = { 1.0f, 1.0f, 0.0f, 1.0f };
		DirectX::XMVECTOR colorStartVar; // calculated in onupdate
		DirectX::XMVECTOR colorEndMin = { 1.0f, 0.0f, 0.0f, 0.0f };
		DirectX::XMVECTOR colorEndMax = { 1.0f, 1.0f, 0.0f, 0.0f };
		DirectX::XMVECTOR colorEndVar; // calculated in onupdate

		DirectX::XMVECTOR scaleStart = { 0.25f, 0.25f, 0.25f, 1.0f };
		DirectX::XMVECTOR scaleEnd   = { 0.01f, 0.01f, 0.01f, 1.0f };

		void InitStartParameters();
		void CreateParticle(Particle* pParticle);
		void UpdateParticle(Particle* pParticle, float dt);
		void InstanceParticle(Particle* pParticle, InstanceInfo* pInstance);
	};
}