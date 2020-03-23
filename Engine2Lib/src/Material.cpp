#include "pch.h"
#include "Material.h"

namespace Engine2
{
	void Material::Bind()
	{
		pVS->Bind();
		pPS->Bind();

		for (auto& r : resources) r->Bind();
	}
}

