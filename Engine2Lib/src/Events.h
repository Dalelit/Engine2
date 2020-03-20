#pragma once
#include "pch.h"

#define E2_BIND_EVENT_FUNC(fn) std::bind(&fn, this, std::placeholders::_1)

namespace Engine2
{

	enum class EventType
	{
		None, WindowResize
	};

	class Event
	{
	public:
		bool handled = false;

		virtual ~Event() = default;

		virtual EventType GetType() = 0;

		virtual const char* GetName() const = 0;

		virtual std::string ToString() const { return GetName(); }

		inline std::ostream& operator<< (std::ostream& stream) { return stream << ToString(); }

	protected:
		EventType type = EventType::None;
	};

	class EventDispatcher
	{
	public:
		EventDispatcher(Event& event) : event(event) {};

		template <typename E, typename F>
		bool Dispatch(const F& func)
		{
			if (event.GetType() == E::GetStaticType())
			{
				event.handled = func(static_cast<E&>(event));
				return true;
			}
			else return false;
		}

	protected:
		Event& event;
	};

	///////////////////// Input events /////////////////////

	class InputEvent : public Event {};


	///////////////////// Applicaiton events /////////////////////

	class ApplicationEvent : public Event {};

	class WindowResizeEvent : public ApplicationEvent
	{
	public:
		WindowResizeEvent(UINT32 width, UINT32 height) : width(width), height(height) {}
		EventType GetType() override { return GetStaticType(); }
		static EventType GetStaticType() { return EventType::WindowResize; }
		const char* GetName() const override { return "WindowResizeEvent"; }

		inline UINT32 GetWidth() { return width; }
		inline UINT32 GetHeight() { return height; }

	protected:
		UINT32 width;
		UINT32 height;
	};

}
