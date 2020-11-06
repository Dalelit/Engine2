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
		virtual ~Material() = default;


		virtual void PreDraw() {} // called before bind and draw
		void Bind();
		virtual void PostDraw() {} // called after bind and draw

		void SetTransform(Transform& transform);

		bool IsValid() { return vertexShaderCB && vertexShader && pixelShader; }

		std::shared_ptr<Material> Clone() { return Clone(name + std::to_string(Assets.Size())); }
		virtual std::shared_ptr<Material> Clone(const std::string& cloneName);

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