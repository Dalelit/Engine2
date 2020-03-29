#pragma once
#include "pch.h"

class Voxel
{
public:
	struct Point {
		float x, y, z;
		char r, g, b;
		char type;
	};

	Voxel(unsigned int width , unsigned int height, unsigned int depth) :
		width(width), height(height), depth(depth), size(width * height * depth)
	{
		data = new Point[size];
		E2_ASSERT(data, "Error allocating point array");
		dataEnd = data + size;
		SetPointData();
	}

	~Voxel()
	{
		delete data;
	}

	unsigned int GetWidth()  { return width; }
	unsigned int GetHeight() { return height; }
	unsigned int GetDepth()  { return depth; }

	Point* begin() { return data; }
	Point* end()   { return dataEnd; }

	unsigned int Size() { return size; }

protected:
	Point* data = nullptr;
	Point* dataEnd = nullptr;

	unsigned int width;
	unsigned int height;
	unsigned int depth;

	unsigned int size;
	size_t pointSize = sizeof(Point);

	void SetPointData()
	{
		Point* current = data;

		float xstart = -(float)width  / 2.0f;
		float ystart = -(float)height / 2.0f;
		float zstart = -(float)depth  / 2.0f;

		Point point = { xstart, ystart, zstart };

		for (unsigned int z = 0; z < depth; z++)
		{
			point.y = ystart;
			for (unsigned int y = 0; y < height; y++)
			{
				point.x = xstart;
				for (unsigned int x = 0; x < width; x++)
				{
					*current = point;
					point.x += 1.0f;
					current++;
				}
				point.y += 1.0f;
			}
			point.z += 1.0f;
		}
	}
};