#pragma once
#include "pch.h"
#include "Common.h"

namespace Engine2
{
	class Surface
	{
	public:
		virtual unsigned int GetWidth()  const = 0;
		virtual unsigned int GetHeight() const = 0;
		virtual unsigned int GetPitch()  const = 0;
		virtual unsigned int GetSlicePitch() const = 0;
		virtual void* GetDataVoidPtr() = 0;
	};

	template <class T>
	class Surface2D : public Surface
	{
	public:

		Surface2D(unsigned int width, unsigned int height) :
			width(width), height(height),
			elementSize(sizeof(T)),
			size(width* height),
			allocatedSize(size),
			pitchBytes(width* elementSize),
			totalBytes(size* elementSize)
		{
			E2_ASSERT(width > 0 && height > 0, "Surface width and height need to be > 0");
			data = new T[allocatedSize];
			E2_ASSERT(data, "Failed to allocate surface data");
		}

		~Surface2D() {
			delete data;
		}

		// overrides
		unsigned int GetWidth()  const { return width; }
		unsigned int GetHeight() const { return height; }
		unsigned int GetPitch() const { return pitchBytes; }
		unsigned int GetSlicePitch() const { return 0u; }
		void* GetDataVoidPtr() { return (void*)data; }

		inline unsigned int GetSize() const { return size; }
		inline unsigned int GetTotalBytes() const { return totalBytes; }

		inline void Clear() { memset(data, 0, totalBytes); }
		inline void Clear(T value) { for (auto first = begin(); first != end(); ++first) *first = value; } //std::for_each(std::execution::par, Begin(), End(), [value](float& val) { val = value; });
	
		inline void SetValue(unsigned int x, unsigned int y, T value) { data[y * width + x] = value; }
		inline T GetValue(unsigned int x, unsigned int y) { return data[y * width + x]; }

		inline T* GetData() const { return data; }
		inline T* GetData(unsigned int x, unsigned int y) { return data + (y * width) + x; }

		inline T* begin() { return data; }
		inline T* end() { return data + size; }


		inline void Copy(Surface2D<T>& src) {
			Resize(src.width, src.height);
			memcpy(data, src.data, totalBytes);
		}

		void Resize(unsigned int newWidth, unsigned int newHeight)
		{
			E2_ASSERT(newWidth > 0 && newHeight > 0, "Resize width and height must be > 0");

			unsigned int newSize = newWidth * newHeight;

			if (newSize > allocatedSize)
			{
				delete data;
				data = new T[newSize];
				allocatedSize = newSize;
			}

			width = newWidth;
			height = newHeight;
			size = newSize;
			pitchBytes = width * elementSize;
			totalBytes = size * elementSize;
		}

	protected:
		T* data = nullptr;

		unsigned int width = 0;
		unsigned int height = 0;
		unsigned int elementSize = 0;
		unsigned int size = 0;
		unsigned int allocatedSize = 0;
		unsigned int pitchBytes = 0;
		unsigned int totalBytes = 0;
	};

}
