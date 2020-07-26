#pragma once

#include "Hazel.h"
#include "ParticleSystem.h"

class Sandbox2D : public Hazel::Layer
{
public:
	Sandbox2D();
	virtual ~Sandbox2D() = default;

	virtual void OnAttach() override;
	virtual void OnDetach() override;

	void OnUpdate(Hazel::Timestep ts) override;
	virtual void OnImGuiRender() override;
	void OnEvent(Hazel::Event& e) override;
private:
	Hazel::OrthographicCameraController m_CameraController;

	// Temp
	Hazel::Ref<Hazel::VertexArray> m_SquareVA;
	Hazel::Ref<Hazel::Shader> m_FlatColorShader;
	Hazel::Ref<Hazel::Framebuffer> m_Framebuffer;

	Hazel::Ref<Hazel::Texture2D> m_RandTexture;

	glm::vec4 m_CustomColor = { 0.2f, 0.3f, 1.0f, 1.0f};

	//ProfileResults to be moved
	struct ProfileResult
	{
		const char* Name;
		float Time;
	};
	std::vector<ProfileResult> m_ProfileResults;

	// Remove these when new particle system is made
	ParticleSystem m_ParticleSystem;
	ParticleProps m_Particle;
};