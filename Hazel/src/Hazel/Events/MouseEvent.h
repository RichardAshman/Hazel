#pragma once

#include "Hazel/Events/Event.h"
#include "Hazel/Core/Input.h"

namespace Hazel {

	class MouseMovedEvent : public Event
	{
	public:
		MouseMovedEvent(float x, float y)
			:m_MouseX(x), m_MouseY(y) {}

		float GetX() const { return m_MouseX; }
		float GetY() const { return m_MouseY; }

		std::string ToString() const override
		{
			std::stringstream ss;

			#if HZ_LOG_EVENTS_TO_CONSOLE
			ss << "MouseMovedEvent: " << m_MouseX << ", " << m_MouseY;
			#endif // HZ_LOG_EVENTS_TO_CONSOLE

			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseMoved)
		EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)
	private:
		float m_MouseX, m_MouseY;
	};

	class MouseScrolledEvent : public Event
	{
	public:
		MouseScrolledEvent(float xOffset, float yOffset)
			:m_XOffset(xOffset), m_YOffset(yOffset) {}

		float GetXOffset() const { return m_XOffset; }
		float GetYOffset() const { return m_YOffset; }

		std::string ToString() const override
		{
			std::stringstream ss;
			#if HZ_LOG_EVENTS_TO_CONSOLE
			ss << "MouseScrolledEvent: " << GetXOffset() << ", " << GetYOffset();
			#endif // HZ_LOG_EVENTS_TO_CONSOLE
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseScrolled)
		EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)
	private:
		float m_XOffset, m_YOffset;
	};

	class MouseButtonEvent : public Event
	{
	public:
		inline MouseCode GetMouseButton() const { return m_Button; }

		EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)
	protected:
		MouseButtonEvent(MouseCode button)
			:m_Button(button) {}

		MouseCode m_Button;
	};

	class MouseButtonPressedEvent : public MouseButtonEvent
	{
	public:
		MouseButtonPressedEvent(MouseCode button)
			: MouseButtonEvent(button){}

		std::string ToString() const override
		{
			std::stringstream ss;
			#if HZ_LOG_EVENTS_TO_CONSOLE
			ss << "MouseButtonPressedEvent: " << m_Button;
			#endif // HZ_LOG_EVENTS_TO_CONSOLE
			return ss.str();
		}
		EVENT_CLASS_TYPE(MouseButtonPressed)
	};

	class MouseButtonReleasedEvent : public MouseButtonEvent
	{
	public:
		MouseButtonReleasedEvent(MouseCode button)
			: MouseButtonEvent(button) {}

		std::string ToString() const override
		{
			std::stringstream ss;
			#if HZ_LOG_EVENTS_TO_CONSOLE
			ss << "MouseButtonReleasedEvent: " << m_Button;
			#endif // HZ_LOG_EVENTS_TO_CONSOLE
			return ss.str();
		}
		EVENT_CLASS_TYPE(MouseButtonReleased)
	};

	// TODO: Make mouse over and mouse off(?) events
}