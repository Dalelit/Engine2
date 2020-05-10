#include "pch.h"
#include "ObjLoader.h"
#include "Common.h"

namespace Engine2
{
	namespace AssetLoaders
	{
		std::shared_ptr<ObjLoader> ObjLoader::Load(std::string filename)
		{
			auto loader = std::make_shared<ObjLoader>();
			loader->filename = filename;

			constexpr size_t reserveCapacity = 100000;
			loader->verticies.reserve(reserveCapacity);
			loader->normals.reserve(reserveCapacity);
			loader->textureCoords.reserve(reserveCapacity);

			LoadObjects(*loader);
			
			return loader;
		}

		void ObjLoader::ScaleVerticies(float scale)
		{
			for (auto& v : verticies) v = { v.x * scale, v.y * scale, v.z * scale };
		}

		// # comment
		// mtllib XXXX.mtl
		// o XXXXObjectNameXXXX
		// v 0.000000 -1.000000 -1.000000
		// vt 0.250000 0.490000
		// vn 0.0000 -1.0000 0.0000
		// usemtl None
		// s off
		// f 1/1/1 12/2/1 2/3/1  --> vertex/texture/normal indices
		//
		void ObjLoader::LoadObjects(ObjLoader& loader)
		{
			std::ifstream srcFile(loader.filename);
			E2_ASSERT(srcFile.is_open(), "Falied to open object file");

			std::string token;
			std::string name;
			std::string currentMat;
			Object* currentObj = nullptr;


			srcFile >> token;
			while (srcFile.good())
			{
				if (token == "#")
				{
					std::getline(srcFile, token); // do nothing for now
				}
				else if (token == "mtllib")
				{
					srcFile >> token; // token = filename
					std::wstring matfilename = std::filesystem::path(Util::ToWString(loader.filename)).replace_filename(Util::ToWString(token));
					LoadMaterials(loader, Util::ToString(matfilename));
				}
				else if (token == "o")
				{
					std::string name;
					srcFile >> name;

					loader.objects[name] = Object();
					currentObj = &loader.objects[name];
					currentObj->name = name;
					constexpr size_t capacity = 50000;
					currentObj->facesV.reserve(capacity);
					currentObj->facesVn.reserve(capacity);
					currentObj->facesVt.reserve(capacity);
				}
				else if (token == "v")
				{
					DirectX::XMFLOAT3 v;
					srcFile >> v.x;
					srcFile >> v.y;
					srcFile >> v.z;
					loader.verticies.push_back(v);
					currentObj->vCount++;
				}
				else if (token == "vt")
				{
					DirectX::XMFLOAT2 vt;
					srcFile >> vt.x;
					srcFile >> vt.y;
					loader.textureCoords.push_back(vt);
					currentObj->vtCount++;
				}
				else if (token == "vn")
				{
					DirectX::XMFLOAT3 vn;
					srcFile >> vn.x;
					srcFile >> vn.y;
					srcFile >> vn.z;
					loader.normals.push_back(vn);
					currentObj->vnCount++;
				}
				else if (token == "usemtl")
				{
					srcFile >> currentObj->material;
				}
				else if (token == "s")
				{
					srcFile >> currentObj->s;
				}
				else if (token == "f")
				{
					// parse function to create a vertex from the index string
					auto parseSubString = [](std::string str, int* values) {
						std::istringstream ss(str);
						std::string num;

						std::getline(ss, num, '/');
						if (!num.empty()) values[0] = std::stoi(num) - 1; else values[0] = -1;
						std::getline(ss, num, '/');
						if (!num.empty()) values[1] = std::stoi(num) - 1; else values[1] = -1;
						std::getline(ss, num, '/');
						if (!num.empty()) values[2] = std::stoi(num) - 1; else values[2] = -1;
					};

					// get the 3 verticies
					std::string faceIndxStr;
					int val[3];
					
					for (int i = 0; i < 3; i++)
					{
						srcFile >> faceIndxStr;
						parseSubString(faceIndxStr, val);
						if (val[0] >= 0) currentObj->facesV.push_back(val[0]);
						if (val[1] >= 0) currentObj->facesVt.push_back(val[1]);
						if (val[2] >= 0) currentObj->facesVn.push_back(val[2]);
					}
				}

				srcFile >> token; // next line
			}
		}

		void ObjLoader::LoadMaterials(ObjLoader& loader, std::string matfilename)
		{
			std::ifstream srcFile(matfilename);
			E2_ASSERT(srcFile.is_open(), "Falied to open material file");

			auto& materials = loader.materials;

			std::string token;
			std::string currentMat;
			DirectX::XMFLOAT3 f3value;
			float fvalue;

			srcFile >> token;

			while (srcFile.good())
			{
				if (token == "#")
				{
					std::getline(srcFile, token); // do nothing for now
				}
				else if (token == "newmtl")
				{
					srcFile >> currentMat;
					materials[currentMat] = Material();
					materials[currentMat].name = currentMat;
				}
				else if (token == "Ka")
				{
					srcFile >> f3value.x;
					srcFile >> f3value.y;
					srcFile >> f3value.z;
					materials[currentMat].Ka = f3value;
					materials[currentMat].attributes.push_back("Ka");
				}
				else if (token == "Kd")
				{
					srcFile >> f3value.x;
					srcFile >> f3value.y;
					srcFile >> f3value.z;
					materials[currentMat].Kd = f3value;
					materials[currentMat].attributes.push_back("Kd");
				}
				else if (token == "Ks")
				{
					srcFile >> f3value.x;
					srcFile >> f3value.y;
					srcFile >> f3value.z;
					materials[currentMat].Ks= f3value;
					materials[currentMat].attributes.push_back("Ks");
				}
				else if (token == "Ns")
				{
					float ns;
					srcFile >> ns;
					materials[currentMat].Ns= ns;
					materials[currentMat].attributes.push_back("Ns");
				}
				else if (token == "Ke")
				{
					srcFile >> f3value.x;
					srcFile >> f3value.y;
					srcFile >> f3value.z;
					materials[currentMat].Ke= f3value;
					materials[currentMat].attributes.push_back("Ke");
				}
				else if (token == "Ni") // index of refraction
				{
					srcFile >> fvalue;
					materials[currentMat].Ni = fvalue;
					materials[currentMat].attributes.push_back("Ni");
				}
				else if (token == "d") // transparency
				{
					srcFile >> fvalue;
					materials[currentMat].d = fvalue;
					materials[currentMat].attributes.push_back("d");
				}
				else if (token == "illum") // illumination mode
				{
					UINT ivalue;
					srcFile >> ivalue;
					materials[currentMat].illum = ivalue;
					materials[currentMat].attributes.push_back("illum");
				}

				srcFile >> token; // next line
			}
		}
	}
}