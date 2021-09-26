#pragma once
#include "Material.h"

namespace Engine2
{
	namespace Materials
	{
		struct LowPolyPSData
		{
			DirectX::XMFLOAT3 baseColor = { 1.0f, 1.0f, 1.0f };
			float padding[1];

			bool OnImgui();
		};

		class LowPolyMaterial : public Material
		{
		public:
			LowPolyMaterial(const std::string& name);
			LowPolyMaterial() : LowPolyMaterial("LowPoly material") {}

			void OnImgui();

			std::shared_ptr<Material> Clone();
			const char* TypeName() { return "LowPolyMaterial"; }

			inline void SetMaterialData(const LowPolyPSData& data) { psData = data; pixelShaderCB.UpdateBuffer(psData); }

		protected:
			LowPolyPSData psData;
		};
	}
}