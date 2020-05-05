#pragma once
#include "pch.h"
#include "VertexBufferInstanced.h"
#include "ConstantBuffer.h"
#include "Shader.h"
#include "Util.h"

namespace Engine2
{
	struct Particle
	{
	public:
		DirectX::XMVECTOR position;
		DirectX::XMVECTOR velocity;
		DirectX::XMVECTOR color;
		DirectX::XMVECTOR scale;
		float life;
	};

	class ParticleEmitter
	{
	public:
		DirectX::XMVECTOR position;
		float rate = 100.0; // per second
		size_t maxParticles = 1000000;

		ParticleEmitter();

		void OnUpdate(float dt);
		void OnRender();
		void OnImgui();

		inline bool IsActive() { return active; }
		inline void SetActive(bool makeActive = true) { active = makeActive; }

		void SetMeshAndVertexShader();
		void SetPixelShader();

	protected:
		bool active = true;
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
		ID3D11Buffer* instanceBuffer;

		std::shared_ptr<DrawableInstanced> pVB;
		std::shared_ptr<VertexShader> pVS;
		std::shared_ptr<PixelShader> pPS;

		Util::Random rng;

		DirectX::XMVECTOR velocityStartMin = { -0.5f, 1.0f, -0.5f, 0.0f };
		DirectX::XMVECTOR velocityStartMax = {  0.5f, 2.0f,  0.5f, 0.0f };
		DirectX::XMVECTOR velocityStartVar; // calculated in onupdate

		DirectX::XMVECTOR force = { 0.0f, -0.3f, 0.0f, 0.0f };

		DirectX::XMVECTOR colorStartMin = { 1.0f, 0.0f, 0.0f, 1.0f };
		DirectX::XMVECTOR colorStartMax = { 1.0f, 1.0f, 0.0f, 1.0f };
		DirectX::XMVECTOR colorStartVar; // calculated in onupdate

		void CreateParticle(Particle* pParticle);
		void UpdateParticle(Particle* pParticle, float dt);
		void InstanceParticle(Particle* pParticle, InstanceInfo* pInstance);
	};
}