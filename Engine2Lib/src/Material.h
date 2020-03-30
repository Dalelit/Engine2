#pragma once

#include "pch.h"
#include "Common.h"
#include "Shader.h"
#include "ConstantBuffer.h"
#include "Texture.h"

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
		std::shared_ptr<Texture> pTexture;

		ResourceVector resources;

		void OnImgui();

	protected:
		std::string name;

	};
}