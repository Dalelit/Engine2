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
			unsigned int s;
			std::vector<unsigned int> facesV;
			std::vector<unsigned int> facesVt;
			std::vector<unsigned int> facesVn;
			unsigned int vCount = 0, vtCount = 0, vnCount = 0;
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

		protected:
			static void LoadObjects(ObjLoader& loader);
			static void LoadMaterials(ObjLoader& loader, std::string matfilename);
		};
	}
}