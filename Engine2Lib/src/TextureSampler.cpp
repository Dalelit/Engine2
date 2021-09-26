#include "pch.h"
#include "TextureSampler.h"

namespace Engine2
{
	TextureSampler::TextureSampler(D3D11_FILTER filter, D3D11_TEXTURE_ADDRESS_MODE addressMode)
	{
		sampDesc.Filter = filter;
		sampDesc.AddressU = addressMode;
		sampDesc.AddressV = addressMode;
		sampDesc.AddressW = addressMode;

		HRESULT hr = DXDevice::GetDevice().CreateSamplerState(&sampDesc, &pSamplerState);

		E2_ASSERT_HR(hr, "CreateSamplerState failed");
	}


	void TextureSampler::OnImgui()
	{
		bool update = false;

		// filter
		{
			const D3D11_FILTER filterValues[] = { D3D11_FILTER_MIN_MAG_MIP_POINT, D3D11_FILTER_MIN_MAG_MIP_LINEAR };
			const char* filterNames[] = { "D3D11_FILTER_MIN_MAG_MIP_POINT", "D3D11_FILTER_MIN_MAG_MIP_LINEAR" };

			int selected = 0;
			while (sampDesc.Filter != filterValues[selected] && selected < ARRAYSIZE(filterValues)) selected++;

			if (ImGui::BeginCombo("Filter", filterNames[selected]))
			{
				for (int i = 0; i < ARRAYSIZE(filterValues); i++)
				{
					bool selected = sampDesc.Filter == filterValues[i];
					if (ImGui::Selectable(filterNames[i], selected))
					{
						sampDesc.Filter = filterValues[i];
						update = true;
					}
				}
				ImGui::EndCombo();
			}
		}

		// address mode
		// to do - allow this to work for UVW individually
		{
			const D3D11_TEXTURE_ADDRESS_MODE modeValues[] = { D3D11_TEXTURE_ADDRESS_WRAP, D3D11_TEXTURE_ADDRESS_MIRROR, D3D11_TEXTURE_ADDRESS_CLAMP, D3D11_TEXTURE_ADDRESS_BORDER, D3D11_TEXTURE_ADDRESS_MIRROR_ONCE };
			const char* modeNames[] = { "D3D11_TEXTURE_ADDRESS_WRAP", "D3D11_TEXTURE_ADDRESS_MIRROR", "D3D11_TEXTURE_ADDRESS_CLAMP", "D3D11_TEXTURE_ADDRESS_BORDER", "D3D11_TEXTURE_ADDRESS_MIRROR_ONCE" };

			int selected = 0;
			while (sampDesc.AddressU != modeValues[selected] && selected < ARRAYSIZE(modeValues)) selected++;

			if (ImGui::BeginCombo("Address mode", modeNames[selected]))
			{
				for (int i = 0; i < ARRAYSIZE(modeValues); i++)
				{
					bool selected = sampDesc.AddressU == modeValues[i];
					if (ImGui::Selectable(modeNames[i], selected))
					{
						sampDesc.AddressU = modeValues[i];
						sampDesc.AddressV = modeValues[i];
						sampDesc.AddressW = modeValues[i];
						update = true;
					}
				}
				ImGui::EndCombo();
			}
		}

		// update - if changed
		if (update)
		{
			HRESULT hr = DXDevice::GetDevice().CreateSamplerState(&sampDesc, &pSamplerState);
			E2_ASSERT_HR(hr, "CreateSamplerState failed");
		}
	}
}