#pragma once
#include "Material.h"

namespace Engine2
{
	// to do: namespace vs class... trying out namespace
	namespace MaterialLibrary
	{
		using VSPtr = std::shared_ptr<VertexShader>;
		using PSPtr = std::shared_ptr<PixelShader>;
		
		// Retrieve existing, or create and stores, a shader
		VSPtr GetVertexShader(const std::string& filename, VertexShaderLayoutDesc& layout);
		PSPtr GetPixelShader(const std::string& filename);

		struct StandardMaterial
		{
			DirectX::XMFLOAT3 ambient = { 1.0f, 1.0f, 1.0f };
			DirectX::XMFLOAT3 diffuse = { 1.0f, 1.0f, 1.0f };
			DirectX::XMFLOAT3 specular = { 1.0f, 1.0f, 1.0f };
			float specularExponent = 5.0f;
			DirectX::XMFLOAT3 emission = { 0.0f, 0.0f, 0.0f };
			float padding[3];
			// For future reference
			//float transparency; DirectX::XMFLOAT3 transmissionFilterColor; float indexOfRefraciton; uint32_t illumination;
		};

		class StandardMaterialVSCB : public VSConstantBuffer<Transform>
		{
		public:
			StandardMaterialVSCB(unsigned int bindSlot) : VSConstantBuffer<Transform>(bindSlot) {}
		};

		class StandardMaterialPSCB : public PSConstantBuffer<StandardMaterial>
		{
		public:
			StandardMaterialPSCB(unsigned int bindSlot) : PSConstantBuffer<StandardMaterial>(bindSlot) {}
			void OnImgui();
		};

		// standard psotion, normal, color material with standard buffer and shaders
		class PositionNormalColorMaterial : public Material
		{
		public:
			PositionNormalColorMaterial(const std::string& name = "PositionNormalColorMaterial");

			StandardMaterial* GetPSCB() { return &((StandardMaterialPSCB*)(pixelShaderCB.get()))->data; }
		};

	};

}
