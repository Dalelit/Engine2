#pragma once
#include "pch.h"

#define E2_BIND_EVENT_FUNC(fn) std::bind(&fn, this, std::placeholders::_1)

namespace Engine2
{

	enum class EventType
	{
		None, WindowResize, MouseMove
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

	class MouseMoveEvent : public InputEvent
	{
	public:
		MouseMoveEvent(UINT32 x, UINT32 y) : x(x), y(y) {}
		EventType GetType() override { return GetStaticType(); }
		static EventType GetStaticType() { return EventType::MouseMove; }
		const char* GetName() const override { return "MouseMove"; }

		int GetX() { return x; }
		int GetY() { return y; }

		std::string ToString() const { return "MouseMove " + std::to_string(x) + "," + std::to_string(y); }

	protected:
		int x, y = 0;
	};


	///////////////////// Applicaiton events /////////////////////

	class ApplicationEvent : public Event {};

	class WindowResizeEvent : public ApplicationEvent
	{
	public:
		WindowResizeEvent(UINT32 width, UINT32 height) : width(width), height(height) {}
		EventType GetType() override { return GetStaticType(); }
		static EventType GetStaticType() { return EventType::WindowResize; }
		const char* GetName() const override { return "WindowResize"; }

		inline UINT32 GetWidth() { return width; }
		inline UINT32 GetHeight() { return height; }

		std::string ToString() const { return "WindowResize " + std::to_string(width) + "," + std::to_string(height); }

	protected:
		UINT32 width, height;
	};

}
