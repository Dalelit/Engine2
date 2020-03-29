#pragma once
#include "pch.h"
#include "ConstantBuffer.h"

namespace Engine2
{
	class Entity
	{
	public:
		Entity(DirectX::XMVECTOR position = { 0.0f, 0.0f, 0.0f, 1.0f},
			   DirectX::XMVECTOR rotation = { 0.0f, 0.0f, 0.0f, 1.0f },
			   DirectX::XMVECTOR scale    = { 1.0f, 1.0f, 1.0f, 1.0f });

		Entity(float x, float y, float z) : Entity({ x, y, z, 1.0f }) {}

		inline void SetPosition(float x, float y, float z) { position = { x, y, z, 1.0f }; }
		inline void SetRotation(float x, float y, float z) { rotation = { x, y, z, 1.0f }; }
		inline void SetScale(float x, float y, float z) { scale = { x, y, z, 1.0f }; }

		inline unsigned long GetID() { return id; }

		inline void SetActive(bool makeActive = true) { active = makeActive; }
		inline bool IsActive() { return active; }

		void LoadTransformT(DirectX::XMMATRIX& invTransform, DirectX::XMMATRIX& invRotationTransform);

		void OnImgui();

	protected:
		static unsigned long entityCounter;

		bool active = true;
		unsigned long id;
		DirectX::XMVECTOR position;
		DirectX::XMVECTOR rotation;
		DirectX::XMVECTOR scale;
	};

	class EntityInstances
	{
	public:
		EntityInstances(unsigned int amountReserved = 1) : vsConstantBuffer(1) { instances.reserve(amountReserved); }

		std::vector<Entity> instances;

		struct VSEntityData
		{
			DirectX::XMMATRIX entityTransform;
			DirectX::XMMATRIX entityTransformRotation;
		};

		static std::string GetVSCBHLSL() {
			return R"(
			cbuffer entityConst : register (b1)
			{
				matrix entityTransform;
				matrix entityTransformRotation;
			};
		)"; }

		VSConstantBuffer<VSEntityData> vsConstantBuffer;
	};
}