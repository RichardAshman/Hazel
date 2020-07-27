#include "hzpch.h"
#include "Hazel/ImGui/ImGuiLayer.h"

#include <imgui.h>
#include <examples/imgui_impl_glfw.h>
#include <examples/imgui_impl_opengl3.h>

#include "Hazel/Core/Application.h"

#include "Hazel/Core/Log.h"
//TEMP
#include <glad/glad.h>
#include <GLFW/glfw3.h>

//#include "Platform/OpenGL/ImGuiOpenGLRenderer.h"

namespace Hazel {
	ImGuiLayer::ImGuiLayer()
		:Layer("ImGuiLayer")
	{
	}

	void ImGuiLayer::OnAttach()
	{
		HZ_PROFILE_FUNCTION();

		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

		ImGuiIO& io = ImGui::GetIO(); (void)io;

		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;		// Enable Keyboard Controls
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;		// Enable Gamepad Controls
			
	        /*
			nb: ImGuiConfigFlags_DockingEnable will not work by default. May need to do two things to fix the issue
			 1) Need to Uncomment the line #define IMGUI_API __declspec( dllexport ) in imconfig.h so that ImGui exports all of its symbols.
			 2) Check if the imgui submodule has been switched to docking rather than master with the command
					C:\Dev\Hazel\Hazel\Vendor\imgui>git status
					if it is set to master then change it to docking with the following commands
					C:\Dev\Hazel\Hazel\Vendor\imgui>git checkout docking
					C:\Dev\Hazel\Hazel\Vendor>git status
					C:\Dev\Hazel\Hazel\Vendor>git add imgui
					C:\Dev\Hazel\Hazel\Vendor\imgui>git submodule update --remote

			*/

		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;			// Enable Docking
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;			// Enable Multi-viewport / Platform Windows
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoTaskBarIcons;
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoMerge;

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();
		// ImGui::StyleColorsClassic();
		
		// When viwports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical ro regular ones.
		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}
		Application& app = Application::Get();
		GLFWwindow* window = static_cast<GLFWwindow*>(app.GetWindow().GetNativeWindow());

		// Setup Platform/Renderer bindings
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init("#version 410");
	}

	void ImGuiLayer::OnDetach()
	{
		HZ_PROFILE_FUNCTION();

		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	void ImGuiLayer::OnEvent(Event& e)
	{
		// TODO: This code needs to be changed. It is stopping the zoom from the scroll
		ImGuiIO& io = ImGui::GetIO();
		e.Handled |= e.IsInCategory(EventCategoryMouse) & io.WantCaptureMouse;
		e.Handled |= e.IsInCategory(EventCategoryKeyboard) & io.WantCaptureKeyboard;
	}
	void ImGuiLayer::Begin()
	{
		HZ_PROFILE_FUNCTION();

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void ImGuiLayer::End()
	{
		HZ_PROFILE_FUNCTION();

		ImGuiIO& io = ImGui::GetIO();
		Application& app = Application::Get();
		io.DisplaySize = ImVec2((float)app.GetWindow().GetWidth(), (float)app.GetWindow().GetHeight());

		// Rendering
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}
	}
}