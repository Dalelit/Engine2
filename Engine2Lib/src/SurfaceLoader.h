#pragma once
#include "Surface.h"

namespace Engine2
{
	class SurfaceLoader
	{
	public:
		static std::shared_ptr<Surface> LoadSurface(const std::string& filename);

		static std::string LastResult; // to do: simple first approach to error handling info

		static constexpr auto Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	};
}