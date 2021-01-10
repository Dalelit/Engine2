#pragma once

#include "pch.h"

namespace Engine2
{
	namespace AssetLoaders
	{
		struct Material
		{
			std::string name;
			float Ns, Ni, d;
			DirectX::XMFLOAT3 Ka, Kd, Ks, Ke;
			int illum;
			std::vector<std::string> attributes;
		};

		struct Object
		{
			std::string name;
			std::string material;
			int s;
			std::vector<unsigned int> facesV;
			std::vector<unsigned int> facesVt;
			std::vector<unsigned int> facesVn;
			unsigned int vCount = 0, vtCount = 0, vnCount = 0;

			inline bool HasPositionNormal() { return !facesV.empty() && !facesVn.empty(); }
			inline bool HasPositionNormalTexture() { return !facesV.empty() && !facesVn.empty() && !facesVt.empty(); }
		};

		class ObjLoader
		{
		public:
			static std::shared_ptr<ObjLoader> Load(std::string filename);

			std::string filename;
			std::map<std::string, Material> materials;
			std::map<std::string, Object> objects;
			std::vector<DirectX::XMFLOAT3> verticies;
			std::vector<DirectX::XMFLOAT3> normals;
			std::vector<DirectX::XMFLOAT2> textureCoords;

			void ScaleVerticies(float scale);

			inline bool IsValid() { return objects.size() > 0 && verticies.size() > 0; }

		protected:
			static void LoadObjects(ObjLoader& loader);
			static void LoadMaterials(ObjLoader& loader, std::string matfilename);
		};
	}
}