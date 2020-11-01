#pragma once
#include "AssetStore.h"
#include "Components.h"
#include "ConstantBuffer.h"
#include "Shader.h"

namespace Engine2
{
	class Material
	{
	public:

		static AssetStore<Material> Assets;

		Material() = default;
		Material(const std::string& name) : name(name) {}

		void Bind();

		void SetTransform(Transform& transform);

		bool IsValid() { return vertexShaderCB && vertexShader && pixelShader; }

		void OnImgui(bool assetInfo = false);

		const std::string& Name() const { return name; }

		std::shared_ptr<VSConstantBuffer<Transform>> vertexShaderCB;
		std::shared_ptr<VertexShader> vertexShader;

		std::shared_ptr<ConstantBufferBase> pixelShaderCB;
		std::shared_ptr<PixelShader> pixelShader;

	protected:
		std::string name;
	};
}