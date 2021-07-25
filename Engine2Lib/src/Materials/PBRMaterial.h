#pragma once
#include "Material.h"

namespace Engine2
{
	namespace Materials
	{
		struct PBRPSData
		{
			DirectX::XMFLOAT3 baseColor = { 1.0f, 1.0f, 1.0f };
			float specular = 0.5f;
			float metallic = 0.0f;
			float roughness = 0.5f;
			float ambient = 0.01f;
			float padding[1];

			bool OnImgui();
		};

		class PBRMaterial : public Material
		{
		public:
			PBRMaterial(const std::string& name);
			PBRMaterial() : PBRMaterial("Standard material") {}

			void OnImgui();

			std::shared_ptr<Material> Clone();

			inline void SetMaterialData(const PBRPSData& data) { psData = data; pixelShaderCB.UpdateBuffer(psData); }

		protected:
			PBRPSData psData;
		};
	}
}