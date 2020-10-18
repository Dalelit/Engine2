#pragma once
#include "Components.h"
#include "Resources.h"
#include "ConstantBuffer.h"
#include "Shader.h"

namespace Engine2
{
	struct Mesh
	{
		std::shared_ptr<Drawable> drawable;
		std::shared_ptr<VSConstantBuffer<Transform>> vertexShaderCB;
		std::shared_ptr<VertexShader> vertexShader;
		std::shared_ptr<PixelShader> pixelShader;

		void OnImgui();

		bool IsValid() { return drawable && vertexShader && vertexShaderCB && pixelShader; }
	};
}
