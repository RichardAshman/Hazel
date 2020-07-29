#pragma once

#include "Hazel.h"

namespace Hazel {

	class EditorLayer : public Hazel::Layer
	{
	public:
		EditorLayer();
		virtual ~EditorLayer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;

		void OnUpdate(Timestep ts) override;
		virtual void OnImGuiRender() override;
		void OnEvent(Event& e) override;
	private:
		Hazel::OrthographicCameraController m_CameraController;

		// Temp
		Ref<VertexArray> m_SquareVA;
		Ref<Shader> m_FlatColorShader;
		Ref<Framebuffer> m_Framebuffer;

		Ref<Texture2D> m_RandTexture;

		glm::vec4 m_CustomColor = { 0.2f, 0.3f, 1.0f, 1.0f };

		bool m_ViewportFocused = false;
		bool m_ViewportHovered = false;
		glm::vec2 m_ViewportSize = {0.0f, 0.0f};

		Ref<Scene> m_ActiveScene;
		Entity m_SquareEntity, m_Square2;


		//TODO: ProfileResults to be moved
		struct ProfileResult
		{
			const char* Name;
			float Time;
		};
		std::vector<ProfileResult> m_ProfileResults;
	};
}