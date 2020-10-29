#pragma once
#include <vector>
#include <map>
#include <fstream>

namespace Engine2
{
	namespace Serialisation
	{

		class Value
		{
		public:
			Value& operator=(const std::string& value) { m_str = value; return *this; }
			Value& operator=(std::string& value) { m_str = value; return *this; }
			Value& operator=(uint32_t value) { m_str = value; return*this; }

		protected:
			std::string m_str;
		};

		class Serialiser
		{
		public:
			class Node
			{
			public:
				Node(const std::string& type, std::ofstream& out, const std::string& indent) :
					m_out(out), m_indent(indent)
				{
					m_out << m_indent << type << std::endl;
					m_indent += "  ";
				}

				Node SubNode(const std::string& type) { return Node(type, m_out, m_indent); }

				void Comment(const std::string& comment) {
					m_out << m_indent << "# " << comment << std::endl;
				}

				template <typename T>
				void Store(const std::string& key, T value) {
					m_out << m_indent << key << ":" << value << std::endl;
				}

				void Store(const std::string& key, DirectX::XMVECTOR value);
				void Store(const std::string& key, DirectX::XMMATRIX value);

			protected:
				std::string m_indent;
				std::ofstream& m_out;
			};

		public:
			Serialiser(const std::string& filename);

			Node GetNode(const std::string& type) { return Node(type, m_out, ""); }

			void Comment(const std::string& comment) {
				m_out << "# " << comment << std::endl;
			}

		protected:
			std::vector<Node> m_nodes;
			std::ofstream m_out;
			std::ifstream m_in;
		};
	}
}
