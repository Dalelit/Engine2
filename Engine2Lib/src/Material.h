#pragma once

#include "pch.h"
#include "Common.h"
#include "Shader.h"
#include "ConstantBuffer.h"

namespace Engine2
{
	class Material
	{
	public:
		Material(std::string name) : name(name) {}
		~Material() = default;
		void Bind();

		std::shared_ptr<Shader> pVS;
		std::shared_ptr<Shader> pPS;
		
		ResourceVector resources;

		void OnImgui();

	protected:
		std::string name;

	};
}