#include <Hazel.h>

// EXAMPLE LAYER
class ExampleLayer : public Hazel::Layer {
public:
	ExampleLayer()
		:Layer("Example")
	{
	}

	void OnUpdate() override
	{
		HZ_INFO("ExampleLayer::Update");
	}
	void OnEvent(Hazel::Event& event) override
	{
		//if (!event.Handled) {
		//	HZ_TRACE("{0}", event);
		//}
	}
};


class Sandbox : public Hazel::Application
{
public: 
	Sandbox()
	{
		PushLayer(new ExampleLayer());
		PushOverlay(new Hazel::ImGuiLayer());

	}
	~Sandbox()
	{

	}
};

Hazel::Application* Hazel::CreateApplication()
{
	return new Sandbox();
}