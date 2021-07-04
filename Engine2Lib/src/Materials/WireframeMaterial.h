#pragma once
#include "Material.h"
#include "Transform.h"

namespace Engine2
{
	namespace Materials
	{
		class WireframeMaterial : public Material
		{
		public:
			WireframeMaterial();

			void OnImgui();

			std::shared_ptr<Material> Clone();

		protected:
			TransformMatrix vsData;
			DirectX::XMFLOAT4 psData = { 0.1f, 0.9f, 0.1f, 1.0f }; // color

			static std::shared_ptr<VertexShader> wireframeVS;
			static std::shared_ptr<PixelShader>  wireframePS;
		};
	}
}
