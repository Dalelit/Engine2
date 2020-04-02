#pragma once

#include "pch.h"

namespace Engine2
{
	class Bindable
	{
	public:
		virtual void Bind() = 0;
		virtual void Unbind() = 0;
		virtual void OnImgui() = 0;
	};

	class Drawable : public Bindable
	{
	public:
		virtual void Draw() = 0;
	};

	typedef std::vector<std::shared_ptr<Bindable>> BindableSharedPtrVector;
	typedef std::vector<std::shared_ptr<Drawable>> DrawableSharedPtrVector;

}