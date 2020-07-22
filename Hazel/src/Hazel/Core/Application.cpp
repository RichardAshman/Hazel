#include "hzpch.h"
#include "Hazel/Core/Application.h"

#include "Hazel/Core/Log.h"
#include "Hazel/Renderer/Renderer.h"

#include "Hazel/Core/Input.h"

#include <GLFW/glfw3.h>

namespace Hazel {

	Application* Application::s_Instance = nullptr;

	Application::Application()
		{
		HZ_PROFILE_FUNCTION();

		HZ_CORE_ASSERT(!s_Instance, "Application already exists!");
		s_Instance = this;

		m_Window = Window::Create();
		m_Window->SetEventCallback(HZ_BIND_EVENT_FN(Application::OnEvent));

		Renderer::Init();

		m_ImGuiLayer = new ImGuiLayer();
		PushOverlay(m_ImGuiLayer);
	}
	Application::~Application()
	{
		Renderer::Shutdown();
	}

	void Application::PushLayer(Layer* layer)
	{
		HZ_PROFILE_FUNCTION();

		m_LayerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay(Layer* overlay)
	{
		HZ_PROFILE_FUNCTION();

		m_LayerStack.PushOverlay(overlay);
		overlay->OnAttach();
	}

	void Application::OnEvent(Event& e)
	{
		HZ_PROFILE_FUNCTION();

		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent> (HZ_BIND_EVENT_FN(Application::OnWindowClose));
		dispatcher.Dispatch<WindowResizeEvent>(HZ_BIND_EVENT_FN(Application::OnWindowResize));

		HZ_CORE_TRACE("OnEvent 1 --> {0}", e);

		for (auto it = m_LayerStack.rbegin(); it != m_LayerStack.rend(); ++it)
		{
			(*it)->OnEvent(e);
			if (e.Handled)
			{
				break;
			}
		}
	}

	void Application::Run()
	{
		HZ_PROFILE_SCOPE("HZ Run Loop");

		while (m_Running)
		{
			float time = (float)glfwGetTime(); //TEMP remove the ref to glfw. Will need to do this in per platform. Platform::GetTime();
			Timestep timestep = time - m_LastFrameTime;
			m_LastFrameTime = time;
			if (!m_Minimized)
			{
				{
					HZ_PROFILE_SCOPE("LayerStack OnUpdate inside run loop");

					for (Layer* layer : m_LayerStack)
					{
						layer->OnUpdate(timestep);
					}
				}
				m_ImGuiLayer->Begin();
				{
					HZ_PROFILE_SCOPE("LayerStack OnImGuiRenderer inside run loop");
					for (Layer* layer : m_LayerStack)
					{
						layer->OnImGuiRender();
					}
				}
				m_ImGuiLayer->End();
			}

			m_Window->OnUpdate(); // should not need to swap buffers if minimized
		}
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		m_Running = false;
		return true;
	}
	bool Application::OnWindowResize(WindowResizeEvent& e)
	{
		HZ_PROFILE_FUNCTION();

		if (e.GetWidth() == 0 || e.GetHeight() == 0)
		{
			m_Minimized = true;
			return false;
		}
		m_Minimized = false;

		Renderer::OnWindowResize(e.GetWidth(), e.GetHeight());

		return false; // No blocking. All layers should be resized.
	}

}