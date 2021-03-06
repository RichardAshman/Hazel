#include "ExampleLayer.h"

#include <imgui/imgui.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

ExampleLayer::ExampleLayer()
	:Layer("Example"), m_CameraController(1280.0f / 720.0f, true) // TODO add max and min zoom into args for camera creation
{
	m_VertexArray = Hazel::VertexArray::Create();

	//Triangle
	float vertices[3 * 7] = { //locationx3 colourx4(RGBA) repeat
		-0.5f, -0.5f, 0.0f, 0.8f, 0.2f, 0.8f, 1.0f,
			0.5f, -0.5f, 0.0f, 0.2f, 0.3f, 0.8f, 1.0f,
			0.0f,  0.5f, 0.0f, 0.8f, 0.8f, 0.2f, 1.0f
	};

	Hazel::Ref<Hazel::VertexBuffer> vertexBuffer = Hazel::VertexBuffer::Create(vertices, sizeof(vertices));
	// Create and set the vertex layout in the form of location(x,y,z) colourx4(R, G, B, A)
	Hazel::BufferLayout layout = {
		{Hazel::ShaderDataType::Float3, "a_Position"},
		{Hazel::ShaderDataType::Float4, "a_Color"}
	};
	vertexBuffer->SetLayout(layout);
	m_VertexArray->AddVertexBuffer(vertexBuffer);
	uint32_t indices[3] = { 0, 1, 2 };
	Hazel::Ref<Hazel::IndexBuffer> indexBuffer = Hazel::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t));
	m_VertexArray->SetIndexBuffer(indexBuffer);

	// Square
	m_SquareVA = Hazel::VertexArray::Create();
	float squareVertices[5 * 4] = { //locationx3, textureCoOrdsx2 repeat 
		-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
			0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
			0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
		-0.5f,  0.5f, 0.0f, 0.0f, 1.0f
	};
	Hazel::Ref<Hazel::VertexBuffer> squareVB = Hazel::VertexBuffer::Create(squareVertices, sizeof(squareVertices));
	squareVB->SetLayout({
		{Hazel::ShaderDataType::Float3, "a_Position"},
		{Hazel::ShaderDataType::Float2, "a_TexCoord"}
		});
	m_SquareVA->AddVertexBuffer(squareVB);
	uint32_t squareIndices[6] = { 0, 1, 2, 2, 3, 0 };
	Hazel::Ref<Hazel::IndexBuffer> squareIB = Hazel::IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t));
	m_SquareVA->SetIndexBuffer(squareIB);


	//Temp first shader test
	std::string vertexSrc = R"(
		#version 330 core

		layout(location = 0) in vec3 a_Position;
		layout(location = 1) in vec4 a_Color;

		uniform mat4 u_ViewProjection;
		uniform mat4 u_Transform;

		out vec3 v_Position;
		out vec4 v_Color;

		void main()
		{
			v_Position = a_Position;
			v_Color = a_Color;
			gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
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

	m_Shader = Hazel::Shader::Create("VertexPosColor", vertexSrc, fragmentSrc);


	Hazel::Shader::Create("assets/shaders/Texture.glsl");

	std::string flatColorShaderVertexSrc = R"(
		#version 330 core

		layout(location = 0) in vec3 a_Position;

		uniform mat4 u_ViewProjection;
		uniform mat4 u_Transform;

		out vec3 v_Position;

		void main()
		{
			v_Position = a_Position;
			gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
		} 
	)";
	std::string flatColorShaderFragmentSrc = R"(
		#version 330 core

		layout(location = 0) out vec4 color;

		uniform vec3 u_Color;

		in vec3 v_Position;

		void main()
		{
			color = vec4(u_Color, 1.0);
		}
	)";

	m_FlatColorShader = Hazel::Shader::Create("FlatColor", flatColorShaderVertexSrc, flatColorShaderFragmentSrc);


	auto textureShader = m_ShaderLibrary.Load("assets/shaders/Texture.glsl");

	m_Texture = Hazel::Texture2D::Create("assets/textures/Rand.png");
	m_Texture2 = Hazel::Texture2D::Create("assets/textures/Rand2.png");

	textureShader->Bind();
	textureShader->SetInt("u_Texture", 0); // texture is bound to slot 0 (next one would be 1 etc)
}

void ExampleLayer::OnAttach()
{
}

void ExampleLayer::OnDetach()
{
}

void ExampleLayer::OnUpdate(Hazel::Timestep ts)
{
	//Update
	m_CameraController.OnUpdate(ts);

	//Renderer
	Hazel::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
	Hazel::RenderCommand::Clear();

	//Renderer::BeginScene(camera, lights, environment); // TODO take these
	Hazel::Renderer::BeginScene(m_CameraController.GetCamera());

	static glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));


	// The current goal for this API is something like:
	// Hazel::MaterialRef material = new Hazel::Material(m_FlatColorShader); 
	// 
	// Hazel::MaterialInstanceRef mi = new Hazel::MaterialInstanceRef(material); 
	// mi->Set("u_Color", redColor); or mi->SetColor(redColor); 
	// mi->Set("u_Texture", texture); or mi->SetTexture(texture);
	// squareMest->SetMaterial(mi);
	// 

	m_FlatColorShader->Bind();
	m_FlatColorShader->SetFloat3("u_Color", m_CustomColor);

	// Submit to be drawn
	for (int y = 0; y < 20; y++)
	{
		for (int x = 0; x < 20; x++)
		{
			glm::vec3 pos(y * 0.11f, x * 0.11f, 0.0f);
			glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos) * scale;
			Hazel::Renderer::Submit(m_FlatColorShader, m_SquareVA, transform);
		}
	}
	// std::dynamic_pointer_cast<OpenGLShader>(shader)


	auto textureShader = m_ShaderLibrary.Get("Texture");

	// Image
	m_Texture->Bind();
	Hazel::Renderer::Submit(textureShader, m_SquareVA, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));

	// Image 2 (with clear background)
	m_Texture2->Bind();
	Hazel::Renderer::Submit(textureShader, m_SquareVA, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));


	// Triangle
	// Hazel::Renderer::Submit(m_Shader, m_VertexArray);

	Hazel::Renderer::EndScene();
	//Renderer::Flush();// For when using multi thread

}
void ExampleLayer::OnImGuiRender()
{
	ImGui::Begin("Settings");
	ImGui::ColorEdit3("Square Color", glm::value_ptr(m_CustomColor));
	ImGui::End();
}
void ExampleLayer::OnEvent(Hazel::Event& e)
{
	m_CameraController.OnEvent(e);
}
