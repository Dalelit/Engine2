#include "pch.h"
#include "Serialiser.h"
#include <fstream>
#include "Common.h"

namespace Engine2
{
	namespace Serialisation
	{
		SaveSerialiser::SaveSerialiser(const std::string& filename)
		{
			m_out = std::ofstream(filename);

			// to do: handle this better
			E2_ASSERT(m_out.good(), "SaveSerialiser bad filename");
		}

		LoadSerialiserStream::LoadSerialiserStream(const std::string& filename)
		{
			m_in = std::ifstream(filename);

			// to do: handle this better
			E2_ASSERT(m_in.good(), "LoadSerialiser bad filename");
		}

		bool LoadSerialiserStream::NextLine()
		{
			while (m_in.good())
			{
				m_spaces = 0;

				std::getline(m_in, m_line);
				++m_lineCount;

				auto current = m_line.begin();
				auto end = m_line.end();

				// count the spaces
				while (current != end && *current == ' ')
				{
					m_spaces++;
					current++;
				}

				// if not an empty line or comment line
				if (current != end && *current != '#')
				{
					// find the ':' split
					auto nameStart = current;
					while (current != end && *current != ':') current++;

					// create the name/value strings
					m_name = std::string(nameStart, current);

					if (current != end) // there is a value
					{
						m_hasValue = true;
						current++;
						auto valueStart = current;
						while (current != end && *current != '#') current++;
						m_value = std::string(valueStart, current);
					}
					else
					{
						m_hasValue = false;
						m_value = "";
					}

					return true;
				}
				// else it will loop to the next line
			}

			return false; // no more lines
		}

		LoadSerialiser::LoadSerialiser(const std::string& filename)
		{
			m_in = std::ifstream(filename);

			// to do: handle this better
			E2_ASSERT(m_in.good(), "LoadSerialiser bad filename");
		}

		bool LoadSerialiser::Load()
		{
			Logging::LogInfo("Loading...");

			std::string line;
			int linecount = 0;
			Node* currentNode = nullptr;

			while (m_in.good())
			{
				std::getline(m_in, line);
				++linecount;

				auto current = line.begin();
				auto end = line.end();
				int spaces = 0;

				while (current != end && *current == ' ')
				{
					spaces++;
					current++;
				}

				if (current != end && *current != '#')
				{
					/////////////////////////////////////////////
					// get the name and the value if it has one

					// find the ':' split
					auto nameStart = current;
					while (current != end && *current != ':') current++;

					// create the name/value strings
					std::string name = std::string(nameStart, current);
					std::string value;
					bool hasValue = false;

					if (current != end) // there is a value
					{
						hasValue = true;
						current++;
						auto valueStart = current;
						while (current != end && *current != '#') current++;
						value = std::string(valueStart, current);
					}

					//////////////////////////////////////////////
					// based on spacing, add to the node, we create a node, or go up to a parent.

					if (spaces == 0)
					{
						// new root node;
						auto& n = nodes.emplace_back(name, spaces, nullptr);
						currentNode = &n;
					}
					else if (hasValue && spaces == currentNode->spaces + 2)
					{
						// same node
						currentNode->attributes[name] = value;
					}
					else if (!hasValue && spaces > currentNode->spaces)
					{
						// new child for the parent
						auto& n = currentNode->children.emplace_back(name, spaces, currentNode);
						currentNode = &n;
					}
					else if (spaces <= currentNode->spaces)
					{
						while (spaces < currentNode->spaces) currentNode = currentNode->parent; // pop back to parent if spaces is <

						// new child
						currentNode = currentNode->parent;
						auto& n = currentNode->children.emplace_back(name, spaces, currentNode);
						currentNode = &n;
					}
					else
					{
						E2_ASSERT(false, "Did not expect to get here");
					}
				}
			}

			Logging::LogInfo("Loaded " + std::to_string(linecount) + " lines");
			return false;
		}

		void WriteNode::Attribute(const char* name, float& value)
		{
			m_out << m_indent << name << ":" << value << std::endl;
		}

		void WriteNode::Attribute(const char* name, int& value)
		{
			m_out << m_indent << name << ":" << value << std::endl;
		}

		void WriteNode::Attribute(const char* name, bool& value)
		{
			m_out << m_indent << name << ":" << (value ? "true" : "false") << std::endl;
		}

		void WriteNode::Attribute(const char* name, size_t& value)
		{
			m_out << m_indent << name << ":" << value << std::endl;
		}

		void WriteNode::Attribute(const char* name, std::string& value)
		{
			m_out << m_indent << name << ":" << value << std::endl;
		}

		void WriteNode::Attribute(const char* name, DirectX::XMFLOAT3& value)
		{
			m_out << m_indent << name << ":" << value.x << "," << value.y << "," << value.z << std::endl;
		}

		void WriteNode::Attribute(const char* name, DirectX::XMVECTOR& value)
		{
			m_out << m_indent << name << ":";
			float* ptr = value.m128_f32;
			m_out << *ptr;
			ptr++;
			for (int i = 1; i < 4; i++)
			{
				m_out << "," << *ptr;
				ptr++;
			}
			m_out << std::endl;
		}

		void WriteNode::Attribute(const char* name, DirectX::XMMATRIX& value)
		{
			m_out << m_indent << name << ":";
			float* ptr = value.r[0].m128_f32;
			m_out << *ptr;
			ptr++;
			for (int i = 1; i < 4 * 4; i++)
			{
				m_out << "," << *ptr;
				ptr++;
			}
			m_out << std::endl;
		}

		WriteNode WriteNode::ChildNode(const char* name)
		{
			m_out << m_indent << name << std::endl;
			return WriteNode(m_indent + m_indentIncrement, m_out);
		}

		void ReadNode::Attribute(const char* name, float& value)
		{
			value = std::stof(m_attributes[name]);
		}

		void ReadNode::Attribute(const char* name, int& value)
		{
			value = std::stoi(m_attributes[name]);
		}

		void ReadNode::Attribute(const char* name, bool& value)
		{
			value = (m_attributes[name] == "true");
		}

		void ReadNode::Attribute(const char* name, size_t& value)
		{
			value = std::stoull(m_attributes[name]);
		}

		void ReadNode::Attribute(const char* name, std::string& value)
		{
			value = m_attributes[name];
		}

		void ReadNode::Attribute(const char* name, DirectX::XMFLOAT3& value)
		{
			std::stringstream src = std::stringstream(m_attributes[name]);
			std::string token;
			int i = 0;

			if (std::getline(src, token, ',')) { value.x = std::stof(token); i++; }
			if (std::getline(src, token, ',')) { value.y = std::stof(token); i++; }
			if (std::getline(src, token, ',')) { value.z = std::stof(token); i++; }

			E2_ASSERT(i == 3, "Expected 3 floats");
		}

		void ReadNode::Attribute(const char* name, DirectX::XMVECTOR& value)
		{
			std::stringstream src = std::stringstream(m_attributes[name]);
			std::string token;
			int i = 0;

			while (std::getline(src, token, ','))
			{
				value.m128_f32[i] = std::stof(token);
				i++;
			}

			E2_ASSERT(i == 4, "Expected 4 floats");
		}

		void ReadNode::Attribute(const char* name, DirectX::XMMATRIX& value)
		{
			std::stringstream src = std::stringstream(m_attributes[name]);
			std::string token;
			int i = 0;

			while (std::getline(src, token, ','))
			{
				float* pf = value.r[0].m128_f32;
				*pf = std::stof(token);
				i++;
			}

			E2_ASSERT(i == 4, "Expected 4 floats");
		}

		void LoadSerialiser::Node::Log()
		{
			std::string tab = "";
			for (int i = 0; i < spaces; i++) tab += "-";

			Logging::LogInfo(tab + name +  " attributes(" + std::to_string(attributes.size()) + ") children(" + std::to_string(children.size()) + ")");
			tab += " ";
			for (auto& [k, v] : attributes) Logging::LogInfo(tab + k + "(" + v + ")");
			for (auto& child : children) child.Log();
		}


	}
}