#include "pch.h"
#include "Serialiser.h"
#include <fstream>
#include "Common.h"

namespace Engine2
{
	namespace Serialisation
	{
		Serialiser::Serialiser(const std::string& filename)
		{
			m_out = std::ofstream(filename);

			// to do: handle this better
			E2_ASSERT(m_out.good(), "SaveToFile bad filename");
		}

		void Serialiser::Node::Store(const std::string& key, DirectX::XMVECTOR value)
		{
			m_out << m_indent << key << ":";
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

		void Serialiser::Node::Store(const std::string& key, DirectX::XMMATRIX value)
		{
			m_out << m_indent << key << ":";
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
	}
}