#pragma once
#include "pch.h"

#define E2_BIND_EVENT_FUNC(fn) std::bind(&fn, this, std::placeholders::_1)

namespace Engine2
{

	enum class EventType
	{
		None, WindowResize, WindowMove, WindowFocus, MouseMove, MouseButtonPressed, MouseButtonReleased, MouseScroll, KeyPressed, KeyReleased
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
		MouseButtonPressedEvent(bool left, bool right, bool middle, UINT32 x, UINT32 y) : left(left), right(right), middle(middle), x(x), y(y) {}
		EventType GetType() override { return GetStaticType(); }
		EventGroup GetGroup() override { return EventGroup::Mouse; }
		static EventType GetStaticType() { return EventType::MouseButtonPressed; }
		const char* GetName() const override { return "MouseButtonPressed"; }

		bool Left() { return left; }
		bool Right() { return right; }
		bool Middle() { return middle; }
		int GetX() { return x; }
		int GetY() { return y; }

		std::string ToString() const { std::stringstream ss; ss << "MouseButtonPressed " << (left ? "left " : "") << (right ? "right " : "") << x << "," << y; return ss.str(); }

	protected:
		bool left, right, middle;
		int x, y;
	};

	class MouseButtonReleasedEvent : public InputEvent
	{
	public:
		MouseButtonReleasedEvent(bool left, bool right, bool middle, UINT32 x, UINT32 y) : left(left), right(right), middle(middle), x(x), y(y) {}
		EventType GetType() override { return GetStaticType(); }
		EventGroup GetGroup() override { return EventGroup::Mouse; }
		static EventType GetStaticType() { return EventType::MouseButtonReleased; }
		const char* GetName() const override { return "MouseButtonReleased"; }

		bool Left() { return left; }
		bool Right() { return right; }
		bool Middle() { return middle; }
		int GetX() { return x; }
		int GetY() { return y; }

		std::string ToString() const { std::stringstream ss; ss << "MouseButtonReleased " << (left ? "left " : "") << (right ? "right " : "") << x << "," << y; return ss.str(); }

	protected:
		bool left, right, middle;
		int x, y;
	};

	class MouseScrollEvent : public InputEvent
	{
	public:
		MouseScrollEvent(int delta, UINT32 x, UINT32 y) : delta(delta), x(x), y(y) {}
		EventType GetType() override { return GetStaticType(); }
		EventGroup GetGroup() override { return EventGroup::Mouse; }
		static EventType GetStaticType() { return EventType::MouseScroll; }
		const char* GetName() const override { return "MouseScroll"; }

		int GetDelta() { return delta; }
		int GetX() { return x; }
		int GetY() { return y; }

		std::string ToString() const { std::stringstream ss; ss << "MouseScroll " << delta << " " << x << "," << y; return ss.str(); }

	protected:
		int delta;
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

		inline UINT32 GetKey() { return key; }

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

	class WindowMoveEvent : public ApplicationEvent
	{
	public:
		WindowMoveEvent(int xPos, int yPos) : x(xPos), y(yPos) {}
		EventType GetType() override { return GetStaticType(); }
		EventGroup GetGroup() override { return EventGroup::Application; }
		static EventType GetStaticType() { return EventType::WindowMove; }
		const char* GetName() const override { return "WindowMove"; }

		int GetX() { return x; }
		int GetY() { return y; }

		std::string ToString() const { std::stringstream ss; ss << "WindowMove " << x << "," << y; return ss.str(); }

	protected:
		int x, y;
	};

	class WindowFocusEvent : public ApplicationEvent
	{
	public:
		WindowFocusEvent(bool activate, bool clicked) : active(activate), clicked(clicked) {}
		EventType GetType() override { return GetStaticType(); }
		EventGroup GetGroup() override { return EventGroup::Application; }
		static EventType GetStaticType() { return EventType::WindowFocus; }
		const char* GetName() const override { return "WindowFocus"; }

		bool IsActive() { return active; }
		bool WasClicked() { return clicked; }
		bool WasKeyboard() { return !clicked; }

		std::string ToString() const { std::stringstream msg; msg << "WindowFocus " << (active ? "Active " : "Inactive ") << (clicked ? "by Click" : "by Keyboard"); return msg.str(); }

	protected:
		bool active;
		bool clicked;
	};

}
