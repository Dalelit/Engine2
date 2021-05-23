#pragma once
#include "ConstantBuffer2.h"
#include "Shader.h"
#include "Texture.h"
#include "UID.h"

namespace Engine2
{
	class Material
	{
	public:

		void Bind() {
			vertexShaderCB.VSBind();
			vertexShader->Bind();
			pixelShaderCB.PSBind();
			pixelShader->Bind();
			if (texture) texture->Bind();
		}

		inline void BindVSCB() { vertexShaderCB.VSBind(); }

		virtual void OnImgui() = 0;

		virtual std::shared_ptr<Material> Clone() = 0;

		const std::string& Name() const { return name; }

		inline UID GetId() const { return id; }
		inline void SetId(UID newId) { id = newId; }

	protected:
		std::string name;
		UID id;

		ConstantBuffer2 vertexShaderCB;
		std::shared_ptr<VertexShader> vertexShader;
		ConstantBuffer2 pixelShaderCB;
		std::shared_ptr<PixelShader> pixelShader;
		std::shared_ptr<Texture> texture;
	};
}