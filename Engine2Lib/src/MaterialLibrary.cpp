#include "pch.h"
#include "Material.h"
#include "MaterialLibrary.h"
#include "VertexLayout.h"

namespace Engine2
{
	namespace MaterialLibrary
	{
		// store of created shaders, indexed off filename

		VSPtr GetVertexShader(const std::string& filename, VertexShaderLayoutDesc& layout)
		{
			// retrieve it if already loaded
			auto existing = Material::VertexShaders.GetAsset(filename);
			if (existing) return existing;

			// create
			VSPtr ptr = std::make_shared<VertexShaderFile>(filename, layout);

			if (!ptr) return nullptr; // failed to load

			// add to the store
			Material::VertexShaders.StoreAsset(filename, ptr);
			
			return ptr;
		}

		PSPtr GetPixelShader(const std::string& filename)
		{
			// retrieve it if already loaded
			auto existing = Material::PixelShaders.GetAsset(filename);
			if (existing) return existing;

			// create
			PSPtr ptr = std::make_shared<PixelShaderFile>(filename);

			if (!ptr) return nullptr; // failed to load

			// add to the store
			Material::PixelShaders.StoreAsset(filename, ptr);

			return ptr;
		}

		void StandardMaterialPSCB::OnImgui()
		{
			if (ImGui::TreeNode("Standard Material PS Constant Buffer"))
			{
				if (ImGui::ColorEdit3("Ambient", &data.ambient.x)) UpdateBuffer();
				if (ImGui::ColorEdit3("Diffuse", &data.diffuse.x)) UpdateBuffer();
				if (ImGui::ColorEdit3("Specular", &data.specular.x)) UpdateBuffer();
				if (ImGui::DragFloat("Exponent", &data.specularExponent)) {
					if (data.specularExponent < 0.001f) data.specularExponent = 0.001f;
					UpdateBuffer();
				}
				if (ImGui::ColorEdit3("Emission", &data.emission.x)) UpdateBuffer();
				ImGui::TreePop();
			}
		}

		PositionNormalColorMaterial::PositionNormalColorMaterial(const std::string& name) : Material(name)
		{
			auto layout = VertexLayout::PositionNormalColor::GetLayout();

			vertexShaderCB = std::make_shared<StandardMaterialVSCB>(1);
			vertexShader = GetVertexShader(Config::directories["EngineShaderSourceDir"] + "PositionNormalColorVS.hlsl", layout);

			pixelShaderCB = std::make_shared<StandardMaterialPSCB>(1);
			pixelShader = GetPixelShader(Config::directories["EngineShaderSourceDir"] + "PositionNormalColorPS.hlsl");
		}

		PositionNormalColorWireframe::PositionNormalColorWireframe(const std::string& name)
		{
			auto layout = VertexLayout::PositionNormalColor::GetLayout();

			vertexShaderCB = std::make_shared<StandardMaterialVSCB>(1);
			vertexShader = GetVertexShader(Config::directories["EngineShaderSourceDir"] + "PositionNormalColorVS.hlsl", layout);

			// simple constant buffer to take 1 color to draw the wireframe
			class PSCB : public PSConstantBuffer<DirectX::XMFLOAT4>
			{
			public:
				PSCB(unsigned int bindSlot) : PSConstantBuffer<DirectX::XMFLOAT4>(bindSlot) {
					data = { 0.1f, 0.8f, 0.1f, 1.0f };
				}

				void OnImgui() {
					if (ImGui::TreeNode("Wireframe PS Constant Buffer")) {
						if (ImGui::ColorEdit3("Color", &data.x)) UpdateBuffer(); // note: edit3 until I decide to put blending in
						ImGui::TreePop();
					}
				}
			protected:
				PSCB* CloneImpl() const { return new PSCB(*this); }
			};

			std::string src = R"(
				cbuffer wireframeConst : register (b1)
				{
					float4 lineColor;
				}

				float4 main(float3 posWS : WSPosition, float3 norWS : WSNormal, float4 col : Color) : SV_TARGET
				{
					return lineColor;
				}
			)";

			pixelShaderCB = std::make_shared<PSCB>(1);
			pixelShader = PixelShader::CreateFromString(src);
		}

		std::shared_ptr<Material> PositionNormalColorWireframe::Clone(const std::string& cloneName)
		{
			auto ptr = Materials.CreateAsset<PositionNormalColorWireframe>(cloneName);

			ptr->vertexShaderCB = std::make_shared<VSConstantBuffer<TransformMatrix>>(*vertexShaderCB); // vertexShaderCB->Clone();
			ptr->vertexShader = vertexShader;
			ptr->pixelShaderCB = pixelShaderCB->Clone();
			ptr->pixelShader = pixelShader;

			return std::shared_ptr<Material>();
		}
	}
}
