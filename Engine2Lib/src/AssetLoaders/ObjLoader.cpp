#include "pch.h"
#include "ObjLoader.h"
#include "Common.h"

namespace Engine2
{
	namespace AssetLoaders
	{
		std::shared_ptr<ObjLoader> ObjLoader::Load(const std::string& rootDirectory, const std::string& filename)
		{
			clock_t startTime = clock();

			auto loader = std::make_shared<ObjLoader>();
			loader->filename = filename;
			loader->rootDirectory = rootDirectory;

			constexpr size_t reserveCapacity = 100000;
			loader->verticies.reserve(reserveCapacity);
			loader->normals.reserve(reserveCapacity);
			loader->textureCoords.reserve(reserveCapacity);

			LoadObjects(*loader);
			
			clock_t endTime = clock();
			E2_LOG_INFO("Load time: " + std::to_string(endTime - startTime) + "ms");

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
		// s off or a number
		// f 1/1/1 12/2/1 2/3/1  --> vertex/texture/normal indices
		//
		void ObjLoader::LoadObjects(ObjLoader& loader)
		{
			std::ifstream srcFile(loader.rootDirectory + "\\" + loader.filename);

			if (!srcFile.is_open()) return;
			//E2_ASSERT(srcFile.is_open(), "Falied to open object file");

			std::string token;
			std::string name;
			std::string currentMat;
			Object* currentObj = nullptr;
			Model* currentModel = nullptr;
			long lineCount = 0;

			srcFile >> token;

			while (srcFile.good())
			{
				lineCount++;

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
					currentModel = &loader.models.emplace_back();
					srcFile >> currentModel->name;
					currentObj = nullptr;
				}
				else if (token == "usemtl" || token == "g") // if not exported with groups, usemtl can be a new object
				{
					std::string objName;
					std::string mtlName;
					bool createNewObject = false;
					
					// To do: is this a hack?
					// a new object for "g" or if we get "usemtl" and the currentObj isn't created, or if created isn't empty
					if (token == "g")
					{
						srcFile >> objName;
						createNewObject = true;
					}
					else
					{
						srcFile >> mtlName;
						if (currentObj == nullptr || currentObj->facesV.size() > 0)
						{
							createNewObject = true;
							objName = currentModel->name + "-" + std::to_string(currentModel->objects.size()) + "-" + mtlName;
						}
					}

					if (createNewObject)
					{
						if (!currentModel) // create the model if it doesn't exist
						{
							currentModel = &loader.models.emplace_back();
							currentModel->name = objName;
						}

						currentModel->objects[objName] = Object();
						currentObj = &currentModel->objects[objName];
						currentObj->name = objName;
						constexpr size_t capacity = 50000;
						currentObj->facesV.reserve(capacity);
						currentObj->facesVn.reserve(capacity);
						currentObj->facesVt.reserve(capacity);
					}

					currentObj->material = mtlName;
				}
				else if (token == "v")
				{
					DirectX::XMFLOAT3 v;
					srcFile >> v.x;
					srcFile >> v.y;
					srcFile >> v.z;
					loader.verticies.push_back(v);
				}
				else if (token == "vt")
				{
					DirectX::XMFLOAT2 vt;
					srcFile >> vt.x;
					srcFile >> vt.y;
					loader.textureCoords.push_back(vt);
				}
				else if (token == "vn")
				{
					DirectX::XMFLOAT3 vn;
					srcFile >> vn.x;
					srcFile >> vn.y;
					srcFile >> vn.z;
					loader.normals.push_back(vn);
				}
				else if (token == "s")
				{
					std::string value;
					srcFile >> value;

					if (value == "off") currentObj->s = 0;
					else currentObj->s = std::stoi(value);
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
			E2_LOG_INFO("Object load line count: " + std::to_string(lineCount));

		}

		void ObjLoader::LoadMaterials(ObjLoader& loader, std::string matfilename)
		{
			std::ifstream srcFile(loader.rootDirectory + "\\" + matfilename);
			E2_ASSERT(srcFile.is_open(), "Falied to open material file");

			auto& materials = loader.materials;

			std::string token;
			std::string currentMat;
			DirectX::XMFLOAT3 f3value;
			float fvalue;
			long lineCount = 0;

			srcFile >> token;

			while (srcFile.good())
			{
				lineCount++;

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

			E2_LOG_INFO("Material load line count: " + std::to_string(lineCount));
		}
	}
}
