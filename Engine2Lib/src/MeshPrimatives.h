#pragma once

#include "pch.h"

namespace Engine2
{
	class MeshPrimatives
	{
	public:

		class Cube
		{
		public:
			static std::vector<DirectX::XMFLOAT3> vertexPositions;
			static std::vector<unsigned int> indicies;
		};
	};
}