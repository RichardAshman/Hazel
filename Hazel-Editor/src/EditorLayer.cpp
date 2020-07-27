#include "hzpch.h"

#include "EditorLayer.h"
#include <imgui/imgui.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//Temp
#include "Platform\OpenGL\OpenGLShader.h"

namespace Hazel {

	EditorLayer::EditorLayer()
		: Layer("EditorLayer"), m_CameraController(1280.0f / 720.0f, true), m_CustomColor({ 0.2f, 0.3f, 0.8f, 1.0f })
	{
	}

	void EditorLayer::OnAttach()
	{
		HZ_PROFILE_FUNCTION();

		m_RandTexture = Hazel::Texture2D::Create("assets/textures/Rand.png");

		Hazel::FramebufferSpecification fbSpec;
		fbSpec.Width = 1280;
		fbSpec.Height = 720;
		m_Framebuffer = Hazel::Framebuffer::Create(fbSpec);
	}

	void EditorLayer::OnDetach()
	{
		HZ_PROFILE_FUNCTION();

	}

	void EditorLayer::OnUpdate(Hazel::Timestep ts)
	{
		HZ_PROFILE_FUNCTION();

		//// Resize
		//if (Hazel::FramebufferSpecification spec = m_Framebuffer->GetSpecification();
		//	m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f && // zero sized framebuffer is invalid
		//	(spec.Width != m_ViewportSize.x || spec.Height != m_ViewportSize.y))
		//{
		//	m_Framebuffer->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		//	m_CameraController.OnResize(m_ViewportSize.x, m_ViewportSize.y);
		//}

		//Update
		m_CameraController.OnUpdate(ts);
		//Renderer
		Hazel::Renderer2D::ResetStats();

		{
			HZ_PROFILE_SCOPE("Renderer Prep");

			m_Framebuffer->Bind();

			Hazel::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
			Hazel::RenderCommand::Clear();
		}

		{
			HZ_PROFILE_SCOPE("Renderer Draw");
			//-z axis is in front // scale factor of 2 will be half the size ie there will be two of the same image (default of 1)
			Hazel::Renderer2D::BeginScene(m_CameraController.GetCamera());
			static float rotation = 0.0f;
			rotation += ts * 500;
			//HZ_CORE_INFO("Frame with ts of {0}", ts.GetMilliseconds());
			Hazel::Renderer2D::DrawRotatedQuad({ -3.0f, 0.0f, 0.1f }, { 0.8f, 0.8f }, glm::radians(45.0f), { 0.8f, 0.2f, 0.3f, 1.0f });
			Hazel::Renderer2D::DrawRotatedQuad({ -1.0f, 0.0f, 0.1f }, { 0.8f, 0.8f }, glm::radians(rotation), { 0.8f, 0.2f, 0.3f, 1.0f });
			Hazel::Renderer2D::DrawRotatedQuad({ -1.0f, 0.0f, 0.1f }, { 0.8f, 0.8f }, glm::radians(-rotation), { 0.1f, 0.2f, 0.3f, 1.0f });

			Hazel::Renderer2D::DrawQuad({ 0.5f, -0.5f }, { 0.5f, 0.75f }, { 0.2f, 0.3f, 0.8f, 1.0f });
			Hazel::Renderer2D::DrawQuad({ 3.0f, 3.0f, 0.1f }, { 1.0f, 1.0f }, m_RandTexture, 1.0f, m_CustomColor);
			Hazel::Renderer2D::DrawQuad({ -3.0f, -3.0f, 0.1f }, { 1.0f, 1.0f }, m_CustomColor);

			for (float y = -5.0f; y < 5.0f; y += 0.5f)
			{
				for (float x = -5.0f; x < 5.0f; x += 0.5f)
				{
					glm::vec4 color = { (x + 5.0f) / 10.0f, 0.4f, (y + 5.0f) / 10.0f, 1.0f };
					Hazel::Renderer2D::DrawQuad({ x, y }, { 0.45f, 0.45f }, color);
				}
			}
			Hazel::Renderer2D::EndScene();
			m_Framebuffer->Unbind();
		}

		if (Hazel::Input::IsMouseButtonPressed(HZ_MOUSE_BUTTON_LEFT))
		{

		}
	}

	void EditorLayer::OnImGuiRender()
	{
		//static bool show = true;
		//ImGui::ShowDemoWindow();

		//example of adding text to imgui
		for (auto& result : m_ProfileResults)
		{
			char label[50];
			strcpy(label, " %.3fms ");
			strcat(label, result.Name);
			ImGui::Text(label, result.Time);
		}
		m_ProfileResults.clear();



		//-----------------------------------------------------------------------------
		// [SECTION] Example App: Docking, DockSpace / ShowExampleAppDockSpace()
		//-----------------------------------------------------------------------------
		// Demonstrate using DockSpace() to create an explicit docking node within an existing window.
		// Note that you already dock windows into each others _without_ a DockSpace() by just moving windows 
		// from their title bar (or by holding SHIFT if io.ConfigDockingWithShift is set).
		// DockSpace() is only useful to construct to a central location for your application.
		static bool dockspaceOpen = true;
		static bool opt_fullscreen_persistant = true;
		bool opt_fullscreen = opt_fullscreen_persistant;
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

		// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
		// because it would be confusing to have two docking targets within each others.
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		if (opt_fullscreen)
		{
			ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->Pos);
			ImGui::SetNextWindowSize(viewport->Size);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		}

		// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background and handle the pass-thru hole, so we ask Begin() to not render a background.
		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
			window_flags |= ImGuiWindowFlags_NoBackground;

		// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
		// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive, 
		// all active windows docked into it will lose their parent and become undocked.
		// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise 
		// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("DockSpace Demo", /*p_open*/&dockspaceOpen, window_flags);
		ImGui::PopStyleVar();

		if (opt_fullscreen)
			ImGui::PopStyleVar(2);

		// DockSpace
		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				// Disabling fullscreen would allow the window to be moved to the front of other windows, 
				// which we can't undo at the moment without finer window depth/z control.
				//ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen_persistant);

				//if (ImGui::MenuItem("Flag: NoSplit", "", (dockspace_flags & ImGuiDockNodeFlags_NoSplit) != 0))                 dockspace_flags ^= ImGuiDockNodeFlags_NoSplit;
				//if (ImGui::MenuItem("Flag: NoResize", "", (dockspace_flags & ImGuiDockNodeFlags_NoResize) != 0))                dockspace_flags ^= ImGuiDockNodeFlags_NoResize;
				if (ImGui::MenuItem("Flag: NoDockingInCentralNode", "", (dockspace_flags & ImGuiDockNodeFlags_NoDockingInCentralNode) != 0))  dockspace_flags ^= ImGuiDockNodeFlags_NoDockingInCentralNode;
				if (ImGui::MenuItem("Flag: PassthruCentralNode", "", (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode) != 0))     dockspace_flags ^= ImGuiDockNodeFlags_PassthruCentralNode;
				if (ImGui::MenuItem("Flag: AutoHideTabBar", "", (dockspace_flags & ImGuiDockNodeFlags_AutoHideTabBar) != 0))          dockspace_flags ^= ImGuiDockNodeFlags_AutoHideTabBar;
				ImGui::Separator();
				if (ImGui::MenuItem("Exit", NULL, false))
				{
					Hazel::Application::Get().Close();
				}
				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}
		ImGui::Begin("Settings");
		auto stats = Hazel::Renderer2D::GetStats();
		ImGui::Text("Renderer2D Stats:");
		ImGui::Text("Draw Calls: %d", stats.DrawCalls);
		ImGui::Text("Quads: %d", stats.QuadCount);
		ImGui::Text("Vertex: %d", stats.GetTotalVertexCount());
		ImGui::Text("Indices: %d", stats.GetTotalIndexCount());
		ImGui::End();

		ImGui::Begin("Settings2");
		ImGui::ColorEdit4("Square Color", glm::value_ptr(m_CustomColor));
		ImGui::End();


		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
		ImGui::Begin("Viewport");
		m_ViewportFocused = ImGui::IsWindowFocused();
		m_ViewportHovered = ImGui::IsWindowHovered();
		Application::Get().GetImGuiLayer()->BlockEvents(!m_ViewportFocused || !m_ViewportHovered);

		ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
		//if (m_ViewportSize != *((glm::vec2*) & viewportPanelSize))
		//if (m_ViewportSize != *((glm::vec2*) & viewportPanelSize))
		//{
		//	m_Framebuffer->Resize((uint32_t)viewportPanelSize.x, (uint32_t)viewportPanelSize.y);
		//	m_ViewportSize = { viewportPanelSize.x, viewportPanelSize.y };
		//	m_CameraController.OnResize(viewportPanelSize.x, viewportPanelSize.y);
		//}
		m_ViewportSize = { viewportPanelSize.x, viewportPanelSize.y };

		uint32_t textureID = m_Framebuffer->GetColorAttachmentRendererID();
		ImGui::Image((void*)textureID, ImVec2{ m_ViewportSize.x, m_ViewportSize.y }, ImVec2{ 0,1 }, ImVec2{1,0});
		ImGui::End();
		ImGui::PopStyleVar();

		ImGui::End();
	}

	void EditorLayer::OnEvent(Hazel::Event& e)
	{
		m_CameraController.OnEvent(e);
	}

}