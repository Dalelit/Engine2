#pragma once
#include <vector>
#include <map>
#include <fstream>
#include "Logging.h"

namespace Engine2
{
	namespace Serialisation
	{
		class INode
		{
		public:
			virtual bool Loading() = 0;
			virtual bool Saving() = 0;

			virtual void Attribute(const char* name, float& value) = 0;
			virtual void Attribute(const char* name, int& value) = 0;
			virtual void Attribute(const char* name, bool& value) = 0;
			virtual void Attribute(const char* name, std::string& value) = 0;
			virtual void Attribute(const char* name, DirectX::XMFLOAT3& value) = 0;
			virtual void Attribute(const char* name, DirectX::XMVECTOR& value) = 0;
			virtual void Attribute(const char* name, DirectX::XMMATRIX& value) = 0;

			virtual void Comment(const char* comment) = 0;
		};

		class WriteNode : public INode
		{
		public:
			WriteNode(const std::string& indent, std::ofstream& out) : m_indent(indent), m_out(out) {}

			bool Loading() { return false; }
			bool Saving() { return true; }

			void Attribute(const char* name, float& value);
			void Attribute(const char* name, int& value);
			void Attribute(const char* name, bool& value);
			void Attribute(const char* name, std::string& value);
			void Attribute(const char* name, DirectX::XMFLOAT3& value);
			void Attribute(const char* name, DirectX::XMVECTOR& value);
			void Attribute(const char* name, DirectX::XMMATRIX& value);

			void Comment(const char* comment) { m_out << m_indent << "# " << comment << std::endl; }

			WriteNode ChildNode(const char* name);

		protected:
			std::string m_indent = "";
			std::string m_indentIncrement = "  ";
			std::ofstream& m_out;
		};

		class ReadNode : public INode
		{
		public:
			ReadNode(std::map<std::string, std::string>& attributes) : m_attributes(attributes) {}

			bool Loading() { return true; }
			bool Saving() { return false; }

			void Attribute(const char* name, float& value);
			void Attribute(const char* name, int& value);
			void Attribute(const char* name, bool& value);
			void Attribute(const char* name, std::string& value);
			void Attribute(const char* name, DirectX::XMFLOAT3& value);
			void Attribute(const char* name, DirectX::XMVECTOR& value);
			void Attribute(const char* name, DirectX::XMMATRIX& value);

			void Comment(const char* comment) {}

		private:
			std::map<std::string, std::string>& m_attributes;
		};

		class SaveSerialiser
		{
		public:
			SaveSerialiser(const std::string& filename);

			WriteNode SaveNode(const char* name) { return WriteNode("", m_out).ChildNode(name); }

			void Comment(const char* comment) { WriteNode("", m_out).Comment(comment); }

		protected:
			std::ofstream m_out;
		};

		class LoadSerialiserStream
		{
		public:
			LoadSerialiserStream(const std::string& filename);

			bool NextLine();

			int Spaces() { return m_spaces; }
			std::string& Name() { return m_name; }
			std::string& Value() { return m_value; }
			bool HasValue() { return m_hasValue; }

		protected:
			std::ifstream m_in;
			int m_spaces;
			std::string m_line;
			std::string m_name;
			std::string m_value;
			bool m_hasValue;
			int m_lineCount = 0;
		};

		class LoadSerialiser
		{
		public:
			class Node
			{
			public:
				Node(const std::string& name, int spaces, Node* parent) : name(name), spaces(spaces), parent(parent) {}
				std::string name;
				std::map<std::string, std::string> attributes; // name:value
				std::vector<Node> children;
				int spaces = 0;
				Node* parent = nullptr;

				ReadNode GetAttributeNode() { return ReadNode(attributes); }

				void Log();
			};

		public:
			LoadSerialiser(const std::string& filename);

			bool Load();

			Node* GetNode(const std::string& name) {
				for (auto& n : nodes) { if (n.name == name) return &n; }
				return nullptr;
			}

			void Log() { for (auto& n : nodes) n.Log(); }

		protected:
			std::ifstream m_in;

			std::vector<Node> nodes;
		};
	}
}
