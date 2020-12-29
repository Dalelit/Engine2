#pragma once
#include "Material.h"
#include "Transform.h"

namespace Engine2
{
	// to do: namespace vs class... trying out namespace
	namespace MaterialLibrary
	{
		struct StandardMaterialData
		{
			DirectX::XMFLOAT3 ambient = { 0.1f, 0.1f, 0.1f }; float pad1;
			DirectX::XMFLOAT3 diffuse = { 1.0f, 1.0f, 1.0f }; float pad2;
			DirectX::XMFLOAT3 specular = { 1.0f, 1.0f, 1.0f };
			float specularExponent = 5.0f;
			DirectX::XMFLOAT3 emission = { 0.0f, 0.0f, 0.0f }; float pad3;
			// For future reference
			//float transparency; DirectX::XMFLOAT3 transmissionFilterColor; float indexOfRefraciton; uint32_t illumination;
		};

		class StandardMaterialVSCB : public VSConstantBuffer<Material::StandardVSData>
		{
		public:
			StandardMaterialVSCB(unsigned int bindSlot) : VSConstantBuffer<Material::StandardVSData>(bindSlot) {}
		protected:
			StandardMaterialVSCB* CloneImpl() const { return new StandardMaterialVSCB(*this); }
		};

		class StandardMaterialPSCB : public PSConstantBuffer<StandardMaterialData>
		{
		public:
			StandardMaterialPSCB(unsigned int bindSlot) : PSConstantBuffer<StandardMaterialData>(bindSlot) {}
			void OnImgui();
		protected:
			StandardMaterialPSCB* CloneImpl() const { return new StandardMaterialPSCB(*this); }
		};

		//////////////////////////////////////////////////////////
		// standard postion, normal, color material with standard buffer and shaders
		class PositionNormalColorMaterial : public Material
		{
		public:
			PositionNormalColorMaterial(const std::string& name = "PositionNormalColorMaterial");

			StandardMaterialData* GetPSCB() { return &((StandardMaterialPSCB*)(pixelShaderCB.get()))->data; }
		};

		//////////////////////////////////////////////////////////
		// Used to wireframe a model
		class PositionNormalColorWireframe : public Material
		{
		public:
			PositionNormalColorWireframe(const std::string& name = "PositionNormalColorWireframe");
			void PreDraw() { DXDevice::Get().SetWireframeRenderState(); } // called before bind and draw
			void PostDraw() { DXDevice::Get().SetDefaultRenderState(); }  // called after bind and draw

			std::shared_ptr<Material> Clone(const std::string& cloneName);
		};
	};

}
