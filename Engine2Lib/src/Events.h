#pragma once
#include "pch.h"

#define E2_BIND_EVENT_FUNC(fn) std::bind(&fn, this, std::placeholders::_1)

namespace Engine2
{

	enum class EventType
	{
		None, WindowResize, MouseMove, MouseButtonPressed, MouseButtonReleased, KeyPressed, KeyReleased
	};

	enum class EventGroup
	{
		None, Application, Mouse, Keyboard
	};

	class Event
	{
	public:
		//bool handled = false; // to do: later when we need for multiple layers

		virtual ~Event() = default;

		virtual EventType GetType() = 0;
		virtual EventGroup GetGroup() = 0;

		virtual const char* GetName() const = 0;

		virtual std::string ToString() const { return GetName(); }

		inline std::ostream& operator<< (std::ostream& stream) { return stream << ToString(); }
	};

	class EventDispatcher
	{
	public:
		EventDispatcher(Event& event) : event(event) {};

		template <typename E, typename F>
		void Dispatch(const F& func)
		{
			if (event.GetType() == E::GetStaticType())
			{
				func(static_cast<E&>(event));
			}
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
		EventGroup GetGroup() override { return EventGroup::Mouse; }
		static EventType GetStaticType() { return EventType::MouseMove; }
		const char* GetName() const override { return "MouseMove"; }

		int GetX() { return x; }
		int GetY() { return y; }

		std::string ToString() const { return "MouseMove " + std::to_string(x) + "," + std::to_string(y); }

	protected:
		int x, y;
	};

	class MouseButtonPressedEvent : public InputEvent
	{
	public:
		MouseButtonPressedEvent(bool left, bool right, UINT32 x, UINT32 y) : left(left), right(right), x(x), y(y) {}
		EventType GetType() override { return GetStaticType(); }
		EventGroup GetGroup() override { return EventGroup::Mouse; }
		static EventType GetStaticType() { return EventType::MouseButtonPressed; }
		const char* GetName() const override { return "MouseButtonPressed"; }

		bool Left() { return left; }
		bool Right() { return right; }
		int GetX() { return x; }
		int GetY() { return y; }

		std::string ToString() const { std::stringstream ss; ss << "MouseButtonPressed " << (left ? "left " : "") << (right ? "right " : "") << x << "," << y; return ss.str(); }

	protected:
		bool left, right;
		int x, y;
	};

	class MouseButtonReleasedEvent : public InputEvent
	{
	public:
		MouseButtonReleasedEvent(bool left, bool right, UINT32 x, UINT32 y) : left(left), right(right), x(x), y(y) {}
		EventType GetType() override { return GetStaticType(); }
		EventGroup GetGroup() override { return EventGroup::Mouse; }
		static EventType GetStaticType() { return EventType::MouseButtonReleased; }
		const char* GetName() const override { return "MouseButtonReleased"; }

		bool Left() { return left; }
		bool Right() { return right; }
		int GetX() { return x; }
		int GetY() { return y; }

		std::string ToString() const { std::stringstream ss; ss << "MouseButtonReleased " << (left ? "left " : "") << (right ? "right " : "") << x << "," << y; return ss.str(); }

	protected:
		bool left, right;
		int x, y;
	};

	class KeyPressedEvent : public InputEvent
	{
	public:
		KeyPressedEvent(UINT32 key, UINT32 repeat) : key(key), repeat(repeat) {}
		EventType GetType() override { return GetStaticType(); }
		EventGroup GetGroup() override { return EventGroup::Keyboard; }
		static EventType GetStaticType() { return EventType::KeyPressed; }
		const char* GetName() const override { return "KeyPressed"; }

		std::string ToString() const { std::stringstream ss; ss << "KeyPressed " << (char)key << " repeat " << repeat; return ss.str(); }

	protected:
		UINT32 key;
		UINT32 repeat;
	};


	///////////////////// Applicaiton events /////////////////////

	class ApplicationEvent : public Event {};

	class WindowResizeEvent : public ApplicationEvent
	{
	public:
		WindowResizeEvent(bool minimised, bool maximised) :minimised(minimised), maximised(maximised) {}
		EventType GetType() override { return GetStaticType(); }
		EventGroup GetGroup() override { return EventGroup::Application; }
		static EventType GetStaticType() { return EventType::WindowResize; }
		const char* GetName() const override { return "WindowResize"; }

		bool IsMinimised() { return minimised; }
		bool IsMaximised() { return maximised; }

		std::string ToString() const { std::string msg = "WindowResize"; if (minimised) msg += " minimised"; else if (maximised) msg += " maximised"; return msg; }

	protected:
		bool minimised;
		bool maximised;
	};

}
