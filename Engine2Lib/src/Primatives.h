#pragma once

#include "pch.h"

namespace Engine2
{
	namespace Primatives
	{
		struct PrimativesVertex
		{
			DirectX::XMFLOAT3 position;
			DirectX::XMFLOAT3 normal;
			DirectX::XMFLOAT4 color;
		};

		struct PrimativesData
		{
			std::shared_ptr<std::vector<PrimativesVertex>> verticies = nullptr;
			std::shared_ptr<std::vector<unsigned int>> indicies = nullptr;
		};

		class Cube
		{
		public:
			static std::vector<PrimativesVertex> verticies;
			static std::vector<unsigned int> indicies;
		};

		class CubeWireframe
		{
		public:
			static std::vector<DirectX::XMFLOAT3> verticies;
			static std::vector<unsigned int> indicies;
		};

		class IcoSphere
		{
		public:
			static PrimativesData CreateIcoSphere(unsigned int iterations);
		};


		template<typename T>
		void CopyPositionNormalColor(std::vector<T>& targetVerticies, std::vector<PrimativesVertex>& primativeVerticies)
		{
			size_t count = primativeVerticies.size();
			if (targetVerticies.size() < count) targetVerticies.resize(count);

			for (auto i = 0; i < count; i++)
			{
				targetVerticies[i].position = primativeVerticies[i].position;
				targetVerticies[i].normal = primativeVerticies[i].normal;
				targetVerticies[i].color = primativeVerticies[i].color;
			}
		}

		template<typename T>
		void CopyPositionNormal(std::vector<T>& targetVerticies, std::vector<PrimativesVertex>& primativeVerticies)
		{
			size_t count = primativeVerticies.size();
			if (targetVerticies.size() < count) targetVerticies.resize(count);

			for (auto i = 0; i < count; i++)
			{
				targetVerticies[i].position = primativeVerticies[i].position;
				targetVerticies[i].normal = primativeVerticies[i].normal;
			}
		}

		template<typename T>
		void CopyPosition(std::vector<T>& targetVerticies, std::vector<PrimativesVertex>& primativeVerticies)
		{
			size_t count = primativeVerticies.size();
			if (targetVerticies.size() < count) targetVerticies.resize(count);

			for (auto i = 0; i < count; i++)
			{
				targetVerticies[i].position = primativeVerticies[i].position;
			}
		}
	};
}