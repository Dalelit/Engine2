#pragma once

#include "pch.h"
#include "Common.h"
#include "Shader.h"

namespace Engine2
{
	class Material
	{
	public:
		Material(std::string name) : name(name) {}
		~Material() = default;
		void Bind();

		std::shared_ptr<VertexShader> pVS;
		std::shared_ptr<PixelShader> pPS;

	protected:
		std::string name;

	};
}