#include "hzpch.h"

#include "Sandbox2D.h"
#include <imgui/imgui.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//Temp
#include "Platform\OpenGL\OpenGLShader.h"


Sandbox2D::Sandbox2D()
	: Layer("Sandbox2D"), m_CameraController(1280.0f / 720.0f, true), m_CustomColor({ 0.2f, 0.3f, 0.8f, 1.0f })
{
}

void Sandbox2D::OnAttach()
{
	HZ_PROFILE_FUNCTION();

	m_RandTexture = Hazel::Texture2D::Create("assets/textures/Rand.png");

	// TODO remove particles from here when new system is made.
	// Init particle here
	m_Particle.ColorBegin = { 254 / 255.0f, 212 / 255.0f, 123 / 255.0f, 1.0f };
	m_Particle.ColorEnd = { 254 / 255.0f, 109 / 255.0f, 41 / 255.0f, 1.0f };
	m_Particle.SizeBegin = 0.3f, m_Particle.SizeVariation = 0.25f, m_Particle.SizeEnd = 0.0f;
	m_Particle.LifeTime = 1.0f;
	m_Particle.Velocity = { 0.0f, 0.0f };
	m_Particle.VelocityVariation = { 2.5f, 1.0f };
	m_Particle.Position = { 0.0f, 0.0f };
}

void Sandbox2D::OnDetach()
{
	HZ_PROFILE_FUNCTION();

}

// TEMP 
static int count = 0;
static float secPerFrame = 0;

void Sandbox2D::OnUpdate(Hazel::Timestep ts)
{
	HZ_PROFILE_FUNCTION();
	//Update
	m_CameraController.OnUpdate(ts);
	//Renderer
	Hazel::Renderer2D::ResetStats();

	//TEMP
	if (count++%10==0) 
	{
		secPerFrame = ts;
	}

	{
		HZ_PROFILE_SCOPE("Renderer Prep");

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
				glm::vec4 color = {(x + 5.0f) / 10.0f, 0.4f, (y + 5.0f) / 10.0f, 1.0f };
				Hazel::Renderer2D::DrawQuad({ x, y }, { 0.45f, 0.45f }, color);
			}
		}
		Hazel::Renderer2D::EndScene();
	}


	if (Hazel::Input::IsMouseButtonPressed(HZ_MOUSE_BUTTON_LEFT))
	{

		// Particle controls
		auto [x, y] = Hazel::Input::GetMousePosition();
		auto width = Hazel::Application::Get().GetWindow().GetWidth();
		auto height = Hazel::Application::Get().GetWindow().GetHeight();

		auto bounds = m_CameraController.GetBounds();
		auto pos = m_CameraController.GetCamera().GetPosition();
		x = (x / width) * bounds.GetWidth() - bounds.GetWidth() * 0.5f;
		y = bounds.GetHeight() * 0.5f - (y / height) * bounds.GetHeight();
		m_Particle.Position = { x + pos.x, y + pos.y };
		for (int i = 0; i < 5; i++) // number of particles per tick
			m_ParticleSystem.Emit(m_Particle);
	}






	//Hazel::Renderer2D::BeginScene(m_CameraController.GetCamera());

	//Hazel::Renderer2D::DrawQuad({ 0.0f, 0.0f, 0.1f }, { 1.0f,1.0f }, m_SpriteSheet);

	//Hazel::Renderer2D::EndScene();




	// Draw particles
	m_ParticleSystem.OnUpdate(ts);
	m_ParticleSystem.OnRender(m_CameraController.GetCamera());
}

void Sandbox2D::OnImGuiRender()
{
	//static bool show = true;
	//ImGui::ShowDemoWindow();

	//example of adding text to imgui
	//ImGui::Begin("m_ProfileResults");
	//for (auto& result : m_ProfileResults)
	//{
	//	char label[50];
	//	strcpy(label, " %.3fms ");
	//	strcat(label, result.Name);
	//	ImGui::Text(label, result.Time);
	//}
	//m_ProfileResults.clear();
	//ImGui::End();

	ImGui::Begin("Settings");
	auto stats = Hazel::Renderer2D::GetStats();
	ImGui::Text("Renderer2D Stats:");
	ImGui::Text("Draw Calls: %d", stats.DrawCalls);
	ImGui::Text("Quads: %d", stats.QuadCount);
	ImGui::Text("Vertex: %d", stats.GetTotalVertexCount());
	ImGui::Text("Indices: %d", stats.GetTotalIndexCount());
	ImGui::Text("frames: %f", secPerFrame);
	ImGui::Text("fps: %f", 1/secPerFrame);
	ImGui::ColorEdit4("Square Color", glm::value_ptr(m_CustomColor));
	ImGui::End();
	
}

void Sandbox2D::OnEvent(Hazel::Event& e)
{
	m_CameraController.OnEvent(e);
}
