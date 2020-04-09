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


		template<typename T>
		void CopyPositionNormalColor(std::vector<T>& verticies, std::vector<PrimativesVertex>& primativeVerticies)
		{
			size_t count = primativeVerticies.size();
			if (verticies.size() < count) verticies.resize(count);

			for (auto i = 0; i < count; i++)
			{
				verticies[i].position = primativeVerticies[i].position;
				verticies[i].normal = primativeVerticies[i].normal;
				verticies[i].color = primativeVerticies[i].color;
			}
		}

		template<typename T>
		void CopyPositionNormal(std::vector<T>& verticies, std::vector<PrimativesVertex>& primativeVerticies)
		{
			size_t count = primativeVerticies.size();
			if (verticies.size() < count) verticies.resize(count);

			for (auto i = 0; i < count; i++)
			{
				verticies[i].position = primativeVerticies[i].position;
				verticies[i].normal = primativeVerticies[i].normal;
			}
		}

		template<typename T>
		void CopyPosition(std::vector<T>& verticies, std::vector<PrimativesVertex>& primativeVerticies)
		{
			size_t count = primativeVerticies.size();
			if (verticies.size() < count) verticies.resize(count);

			for (auto i = 0; i < count; i++)
			{
				verticies[i].position = primativeVerticies[i].position;
			}
		}
	};
}