#pragma once
#include "AssetStore.h"
#include "Components.h"
#include "Transform.h"
#include "ConstantBuffer.h"
#include "Shader.h"
#include "Texture.h"

namespace Engine2
{
	class Material
	{
	public:

		static AssetStore<Material>         Materials;
		static AssetStore<VertexShaderFile> VertexShaders;
		static AssetStore<PixelShaderFile>  PixelShaders;

		// Retrieve existing, or create and stores, a shader
		static std::shared_ptr<VertexShaderFile> GetVertexShader(const std::string& filename, VertexShaderLayoutDesc& layout);
		static std::shared_ptr<PixelShaderFile> GetPixelShader(const std::string& filename);

		Material() = default;
		Material(const std::string& name) : name(name) {}
		virtual ~Material() = default;


		virtual void PreDraw() {} // called before bind and draw
		void Bind();
		void ShadowBind();
		virtual void PostDraw() {} // called after bind and draw

		void SetTransform(TransformMatrix& transform);

		bool IsValid() { return vertexShaderCB && vertexShader && pixelShader; }

		std::shared_ptr<Material> Clone() { return Clone(name + std::to_string(Materials.Size())); }
		virtual std::shared_ptr<Material> Clone(const std::string& cloneName);

		void OnImgui();

		const std::string& Name() const { return name; }

		struct StandardVSData {
			DirectX::XMMATRIX rotationMatrix;
			DirectX::XMMATRIX transformMatrix;

			inline StandardVSData& operator=(const TransformMatrix& rhs) {
				rotationMatrix = rhs.GetRotationTransposed();
				transformMatrix = rhs.GetTransformTransposed();
				return *this;
			}
		};

		std::shared_ptr<VSConstantBuffer<StandardVSData>> vertexShaderCB;
		std::shared_ptr<VertexShader> vertexShader;

		std::shared_ptr<ConstantBufferBase> pixelShaderCB;
		std::shared_ptr<PixelShader> pixelShader;

		std::shared_ptr<Texture> texture;

	protected:
		std::string name;
	};
}