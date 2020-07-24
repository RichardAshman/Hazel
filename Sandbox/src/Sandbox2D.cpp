#include "hzpch.h"

#include "Sandbox2D.h"
#include <imgui/imgui.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//Temp
#include "Platform\OpenGL\OpenGLShader.h"


Sandbox2D::Sandbox2D()
	: Layer("Sandbox2D"), m_CameraController(1280.0f / 720.0f, true)
{
}

void Sandbox2D::OnAttach()
{
	HZ_PROFILE_FUNCTION();

	m_transparentTexture = Hazel::Texture2D::Create("assets/textures/Rand.png");
	//m_Texture = Hazel::Texture2D::Create("assets/textures/Rand2.png");
}

void Sandbox2D::OnDetach()
{
	HZ_PROFILE_FUNCTION();

}

void Sandbox2D::OnUpdate(Hazel::Timestep ts)
{
	HZ_PROFILE_FUNCTION();
	//Update
	m_CameraController.OnUpdate(ts);
	//Renderer
	{
		HZ_PROFILE_SCOPE("Renderer Prep");
		Hazel::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
		Hazel::RenderCommand::Clear();
	}

	{
		HZ_PROFILE_SCOPE("Renderer Draw");
		static float rotation = 0.0f;
		rotation += ts * 50;

		Hazel::Renderer2D::BeginScene(m_CameraController.GetCamera());
		/*
		//-z axis is in front // scale factor of 2 will be half the size ie there will be two of the same image (default of 1)
		Hazel::Renderer2D::DrawRotatedQuad({ -1.0f, 0.0f }, { 0.8f, 0.8f }, glm::radians(-45.0f), { 0.8f, 0.2f, 0.3f, 1.0f });
		Hazel::Renderer2D::DrawQuad({ 0.5f, -0.5f }, { 0.5f, 0.75f }, { 0.2f, 0.3f, 0.8f, 1.0f });
		Hazel::Renderer2D::DrawQuad({ 0.0f, 0.0f, -0.1f}, { 10.0f, 10.0f }, m_transparentTexture); 
		*/

		Hazel::Renderer2D::DrawRotatedQuad({ -1.0f, 0.0f, 0.1f }, { 0.8f, 0.8f }, rotation, { 0.8f, 0.2f, 0.3f, 1.0f });

		Hazel::Renderer2D::DrawRotatedQuad({ -1.0f, 0.0f, 0.1f }, { 0.8f, 0.8f }, -rotation, { 0.1f, 0.2f, 0.3f, 1.0f });

		//Hazel::Renderer2D::DrawQuad({ -1.0f, 0.0f, 0.5f}, { 0.8f, 0.8f }, { 0.8f, 0.2f, 0.3f, 1.0f });
		//Hazel::Renderer2D::DrawQuad({ 0.5f, -0.5f }, { 0.5f, 0.75f }, { 0.2f, 0.3f, 0.8f, 1.0f });

		//Hazel::Renderer2D::DrawQuad({ 0.5f, -0.5f, 0.4f }, { 0.5f, 0.75f }, m_transparentTexture);

		Hazel::Renderer2D::DrawQuad({ -2.0f, -5.0f, -0.1f}, { 10.0f, 10.0f }, m_transparentTexture, 1); 
		//Hazel::Renderer2D::DrawQuad({ -5.0f, -2.0f, -0.2f}, { 10.0f, 10.0f }, m_transparentTexture, 1); 

		Hazel::Renderer2D::EndScene();
	}
}

void Sandbox2D::OnImGuiRender()
{
	ImGui::Begin("Settings");
	ImGui::ColorEdit4("Square Color", glm::value_ptr(m_SquareColor));
	// example of adding text to imgui
	//for (auto& result : m_ProfileResults)
	//{
	//	char label[50];
	//	strcpy(label, " %.3fms ");
	//	strcat(label, result.Name);
	//	ImGui::Text(label, result.Time);
	//}
	//m_ProfileResults.clear();

	ImGui::End();
}

void Sandbox2D::OnEvent(Hazel::Event& e)
{
	m_CameraController.OnEvent(e);
}
