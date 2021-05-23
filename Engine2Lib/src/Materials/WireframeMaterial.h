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
			DirectX::XMFLOAT4 psData = {}; // color

			static std::shared_ptr<PixelShader> colorPS;
		};
	}
}
