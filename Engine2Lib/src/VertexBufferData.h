#pragma once
#include <stdint.h>

namespace Engine2
{
	// Simple data store for vertex data (no indicies)
	template <typename V> // vertex type
	class VertexBufferData
	{
	public:

		VertexBufferData() {}
		~VertexBufferData() { if (m_data) delete[] m_data; }

		void Resize(size_t vertexCount)
		{
			assert(vertexCount > 0 && "VertexBufferData invalid size");

			size_t vertexSize = vertexCount * sizeof(V);

			if (m_reserved < vertexSize)
			{
				if (m_data) delete[] m_data;

				m_reserved = vertexSize;
				m_data = new char[m_reserved];
			}

			m_vertexCount = vertexCount;
			m_dataEnd = m_data + vertexSize;
		}

		inline size_t VertexCount() { return m_vertexCount; }

		inline V* VertexBegin() { return (V*)m_data; }
		inline V* VertexEnd() { return (V*)m_dataEnd; }

	protected:
		size_t m_vertexCount = 0;

		size_t m_reserved = 0;
		char* m_data = nullptr;
		char* m_dataEnd = nullptr;
	};



	// Simple data store for vertex and index data
	template <typename V, typename I = uint32_t> // vertex type, index type (defaulted to the usual)
	class VertexIndexBufferData
	{
	public:

		VertexIndexBufferData() {}
		~VertexIndexBufferData() { if (m_data) delete[] m_data; }

		void Resize(size_t vertexCount, size_t indexCount)
		{
			assert(vertexCount > 0 && indexCount > 0 && "VertexBufferData invalid size");

			size_t vertexSize = vertexCount * sizeof(V);
			size_t indexSize = indexCount * sizeof(I);

			if (m_reserved < vertexSize + indexSize)
			{
				if (m_data) delete[] m_data;

				m_reserved = vertexSize + indexSize;
				m_data = new char[m_reserved];
			}

			m_vertexCount = vertexCount;
			m_indexCount = indexCount;
			m_dataIndex = m_data + vertexSize;
			m_dataEnd = m_dataIndex + indexSize;
		}

		inline size_t VertexCount() { return m_vertexCount; }
		inline size_t IndexCount() { return m_indexCount; }

		inline V* VertexBegin() { return (V*)m_data; }
		inline V* VertexEnd() { return (V*)m_dataIndex; }

		inline I* IndexBegin() { return (I*)m_dataIndex; }
		inline I* IndexEnd() { return (I*)m_dataEnd; }

	protected:
		size_t m_vertexCount = 0;
		size_t m_indexCount = 0;

		size_t m_reserved = 0;
		char* m_data = nullptr;
		char* m_dataIndex = nullptr;
		char* m_dataEnd = nullptr;
	};
}