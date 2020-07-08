#include <Hazel.h>

#include "imgui/imgui.h"

// EXAMPLE LAYER
class ExampleLayer : public Hazel::Layer {
public:
	ExampleLayer()
		:Layer("Example"), m_Camera(-1.6f, 1.6f, -0.9f, 0.9f), m_CameraPosition(0.0f) // numbers are currently hardcoded for aspect ratio
	{
		m_VertexArray.reset(Hazel::VertexArray::Create());

		//Triangle
		float vertices[3 * 7] = { //locationx3 colourx4(RGBA) repeat
			-0.5f, -0.5f, 0.0f, 0.8f, 0.2f, 0.8f, 1.0f,
			 0.5f, -0.5f, 0.0f, 0.2f, 0.3f, 0.8f, 1.0f,
			 0.0f,  0.5f, 0.0f, 0.8f, 0.8f, 0.2f, 1.0f
		};

		std::shared_ptr<Hazel::VertexBuffer> vertexBuffer;
		vertexBuffer.reset(Hazel::VertexBuffer::Create(vertices, sizeof(vertices)));
		// Create and set the vertex layout in the form of location(x,y,z) colourx4(R, G, B, A)
		Hazel::BufferLayout layout = {
			{Hazel::ShaderDataType::Float3, "a_Position"},
			{Hazel::ShaderDataType::Float4, "a_Color"}
		};
		vertexBuffer->SetLayout(layout);
		m_VertexArray->AddVertexBuffer(vertexBuffer);

		uint32_t indices[3] = { 0, 1, 2 };
		std::shared_ptr<Hazel::IndexBuffer> indexBuffer;
		indexBuffer.reset(Hazel::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t)));
		m_VertexArray->SetIndexBuffer(indexBuffer);

		// square test
		m_SquareVA.reset(Hazel::VertexArray::Create());
		float squareVertices[3 * 4] = { //locationx3 colourx4(RGBA) repeat
			-0.75f, -0.75f, 0.0f,
			 0.75f, -0.75f, 0.0f,
			 0.75f,  0.75f, 0.0f,
			-0.75f,  0.75f, 0.0f
		};
		std::shared_ptr<Hazel::VertexBuffer> squareVB;
		squareVB.reset(Hazel::VertexBuffer::Create(squareVertices, sizeof(squareVertices)));
		squareVB->SetLayout({
			{Hazel::ShaderDataType::Float3, "a_Position"}
			});
		m_SquareVA->AddVertexBuffer(squareVB);
		uint32_t squareIndices[6] = { 0, 1, 2, 2, 3, 0 };
		std::shared_ptr<Hazel::IndexBuffer> squareIB;
		squareIB.reset(Hazel::IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t)));
		m_SquareVA->SetIndexBuffer(squareIB);


		//Temp first shader test
		std::string vertexSrc = R"(
			#version 330 core

			layout(location = 0) in vec3 a_Position;
			layout(location = 1) in vec4 a_Color;

			uniform mat4 u_ViewProjection;

			out vec3 v_Position;
			out vec4 v_Color;

			void main()
			{
				v_Position = a_Position;
				v_Color = a_Color;
				gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
			}
		)";

		std::string fragmentSrc = R"(
			#version 330 core

			layout(location = 0) out vec4 color;

			in vec3 v_Position;
			in vec4 v_Color;

			void main()
			{
				color = vec4(v_Position*0.5+0.5,1.0);
				color = v_Color;
			}
		)";

		m_Shader.reset(new Hazel::Shader(vertexSrc, fragmentSrc));


		//Blue square shader test
		std::string blueShaderVertexSrc = R"(
			#version 330 core

			layout(location = 0) in vec3 a_Position;

			uniform mat4 u_ViewProjection;

			out vec3 v_Position;

			void main()
			{
				v_Position = a_Position;
				gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
			} 
		)";
		std::string blueShaderFragmentSrc = R"(
			#version 330 core

			layout(location = 0) out vec4 color;

			in vec3 v_Position;

			void main()
			{
				color = vec4(0.2, 0.3, 0.8, 1.0);
			}
		)";

		m_BlueShader.reset(new Hazel::Shader(blueShaderVertexSrc, blueShaderFragmentSrc));
	}

	void OnUpdate() override
	{


		Hazel::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
		Hazel::RenderCommand::Clear();

		m_Camera.SetPosition(m_CameraPosition);// { 0.5f, 0.5f, 0.0f });
		m_Camera.SetRotation(0.0f);
		//Renderer::BeginScene(camera, lights, environment); // TODO take these
		Hazel::Renderer::BeginScene(m_Camera);

		// Submit to be drawn
		Hazel::Renderer::Submit(m_BlueShader, m_SquareVA);
		Hazel::Renderer::Submit(m_Shader, m_VertexArray);

		Hazel::Renderer::EndScene();
		//Renderer::Flush();// For when using multi thread





		//HZ_INFO("ExampleLayer::Update");
		/*
		// Used to test tab key and polling
		if (Hazel::Input::IsKeyPressed(HZ_KEY_TAB))
		{
			HZ_TRACE("Tab key is pressed (poll)!");
		}
		*/
	}
	virtual void OnImGuiRender() override
	{
		////Test for ImGui window
		//ImGui::Begin("Test");
		//ImGui::Text("Hello Would");
		//ImGui::End();
	}
	void OnEvent(Hazel::Event& event) override
	{
		Hazel::EventDispatcher dispatcher(event);
		dispatcher.Dispatch<Hazel::KeyPressedEvent>(HZ_BIND_EVENT_FN(ExampleLayer::OnkeyPressedEvent));



		//if (!event.Handled) {
		//	HZ_TRACE("{0}", event);
		//}
		//if (event.GetEventType() == Hazel::EventType::KeyPressed)
		//{
		//	Hazel::KeyPressedEvent& e = (Hazel::KeyPressedEvent&)event;
		//	if (e.GetKeyCode() == HZ_KEY_TAB)
		//		HZ_TRACE("Tab key is pressed (event)!");
		//	HZ_TRACE("{0}", (char)e.GetKeyCode());
		//}
	}
	bool OnkeyPressedEvent(Hazel::KeyPressedEvent& event)
	{
		if (event.GetKeyCode() == HZ_KEY_LEFT)
		{
			m_CameraPosition.x -= m_CameraSpeed;
		}
		if (event.GetKeyCode() == HZ_KEY_RIGHT)
		{
			m_CameraPosition.x += m_CameraSpeed;
		}
		if (event.GetKeyCode() == HZ_KEY_UP)
		{
			m_CameraPosition.y += m_CameraSpeed;
		}
		if (event.GetKeyCode() == HZ_KEY_DOWN)
		{
			m_CameraPosition.y -= m_CameraSpeed;
		}

		return false;
	}
private:

	std::shared_ptr<Hazel::Shader> m_Shader;
	std::shared_ptr<Hazel::VertexArray> m_VertexArray;

	std::shared_ptr<Hazel::Shader> m_BlueShader;
	std::shared_ptr<Hazel::VertexArray> m_SquareVA;

	Hazel::OrthographicCamera m_Camera;
	glm::vec3 m_CameraPosition;
	float m_CameraSpeed = 0.1f;
};

class Sandbox : public Hazel::Application
{
public: 
	Sandbox()
	{
		//PushLayer(new ExampleLayer());
	}
	~Sandbox()
	{

	}
};

Hazel::Application* Hazel::CreateApplication()
{
	return new Sandbox();
}