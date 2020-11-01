#include "pch.h"
#include "Material.h"
#include "MaterialLibrary.h"
#include "VertexLayout.h"

namespace Engine2
{
	namespace MaterialLibrary
	{
		// store of created shaders, indexed off filename
		std::unordered_map<std::string, VSPtr> vertexShaders;
		std::unordered_map<std::string, PSPtr> pixelShaders;

		VSPtr GetVertexShader(const std::string& filename, VertexShaderLayoutDesc& layout)
		{
			// retrieve it if already loaded
			auto existing = vertexShaders.find("filename");
			if (existing != vertexShaders.end()) return existing->second;

			// create
			VSPtr ptr = VertexShader::CreateFromSourceFile(filename, layout);

			if (!ptr) return nullptr; // failed to load

			// add to the store
			vertexShaders.insert({ filename, ptr });
			
			return ptr;
		}

		PSPtr GetPixelShader(const std::string& filename)
		{
			// retrieve it if already loaded
			auto existing = pixelShaders.find("filename");
			if (existing != pixelShaders.end()) return existing->second;

			// create
			PSPtr ptr = PixelShader::CreateFromSourceFile(filename);

			if (!ptr) return nullptr; // failed to load

			// add to the store
			pixelShaders.insert({ filename, ptr });

			return ptr;
		}

		void StandardMaterialPSCB::OnImgui()
		{
			if (ImGui::TreeNode("Standard Material PS Constant Buffer"))
			{
				if (ImGui::ColorEdit3("Ambient", &data.ambient.x)) UpdateBuffer();
				if (ImGui::ColorEdit3("Diffuse", &data.diffuse.x)) UpdateBuffer();
				if (ImGui::ColorEdit3("Specular", &data.specular.x)) UpdateBuffer();
				if (ImGui::DragFloat("Exponent", &data.specularExponent)) UpdateBuffer();
				if (ImGui::ColorEdit3("Emission", &data.emission.x)) UpdateBuffer();
				ImGui::TreePop();
			}
		}

		PositionNormalColorMaterial::PositionNormalColorMaterial(const std::string& name) : Material(name)
		{
			auto layout = VertexLayout::PositionNormalColor::GetLayout();

			// To Do: move the hlsl's into proper locations

			vertexShaderCB = std::make_shared<StandardMaterialVSCB>(1);
			vertexShader = GetVertexShader("Assets\\Materials\\StandardPosNorColVS.hlsl", layout);

			pixelShaderCB = std::make_shared<StandardMaterialPSCB>(1);
			pixelShader = GetPixelShader("Assets\\Materials\\StandardPosNorColPS.hlsl");
		}

	}
}
