#pragma once
#include "Material.h"

namespace Engine2
{
	namespace Materials
	{
		struct StandardPSData
		{
			DirectX::XMFLOAT3 ambient = { 0.1f, 0.1f, 0.1f }; float pad1;
			DirectX::XMFLOAT3 diffuse = { 1.0f, 1.0f, 1.0f }; float pad2;
			DirectX::XMFLOAT3 specular = { 1.0f, 1.0f, 1.0f };
			float specularExponent = 5.0f;
			DirectX::XMFLOAT3 emission = { 0.0f, 0.0f, 0.0f }; float pad3;
			// For future reference
			//float transparency; DirectX::XMFLOAT3 transmissionFilterColor; float indexOfRefraciton; uint32_t illumination;

			bool OnImgui();
		};

		class StandardMaterial : public Material
		{
		public:
			StandardMaterial(const std::string& name);
			StandardMaterial() : StandardMaterial("Standard material") {}

			void OnImgui();

			std::shared_ptr<Material> Clone();
			const char* TypeName() { return "StandardMaterial"; }

			inline void SetMaterialData(const StandardPSData& data) { psData = data; pixelShaderCB.UpdateBuffer(psData); }

		protected:
			StandardPSData psData;
		};
	}
}