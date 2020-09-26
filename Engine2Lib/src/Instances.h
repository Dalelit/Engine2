#pragma once
#include "pch.h"
#include "ConstantBuffer.h"

namespace Engine2
{
	class Instance
	{
	public:
		Instance(DirectX::XMVECTOR position = { 0.0f, 0.0f, 0.0f, 1.0f },
			DirectX::XMVECTOR rotation = { 0.0f, 0.0f, 0.0f, 1.0f },
			DirectX::XMVECTOR scale = { 1.0f, 1.0f, 1.0f, 1.0f });

		Instance(float x, float y, float z) : Instance({ x, y, z, 1.0f }) {}

		inline void SetPosition(float x, float y, float z) { position = { x, y, z, 1.0f }; }
		inline void SetRotation(float x, float y, float z) { rotation = { x, y, z, 1.0f }; }
		inline void SetScale(float x, float y, float z) { scale = { x, y, z, 1.0f }; }

		inline void SetActive(bool makeActive = true) { active = makeActive; }
		inline bool IsActive() { return active; }

		void LoadTransformT(DirectX::XMMATRIX& invTransform, DirectX::XMMATRIX& invRotationTransform);

		void OnImgui();

	protected:
		static unsigned long InstanceCounter;

		bool active = true;
		std::string id;
		DirectX::XMVECTOR position;
		DirectX::XMVECTOR rotation;
		DirectX::XMVECTOR scale;
	};

	class Instances
	{
	public:
		Instances(unsigned int amountReserved = 1) : vsConstantBuffer(1) { instances.reserve(amountReserved); }

		std::vector<Instance> instances;

		struct VSInstanceData
		{
			DirectX::XMMATRIX transform;
			DirectX::XMMATRIX transformRotation;
		};

		static std::string GetVSCBHLSL() {
			return R"(
			cbuffer InstanceConst : register (b1)
			{
				matrix instanceTransform;
				matrix instanceTransformRotation;
			};
		)";
		}

		VSConstantBuffer<VSInstanceData> vsConstantBuffer;
	};
}
