#include "hzpch.h"
#include "Components.h"
#include "Scene.h"

#include "Hazel/Renderer/Renderer2D.h"

#include <glm/glm.hpp>

namespace Hazel
{
	Scene::Scene()
	{
	}
	Scene::~Scene()
	{
	}
	entt::entity Scene::CreateEntity()
	{
		return m_Reg.create();
	}
	void Scene::OnUpdate(Timestep ts)
	{
		auto group = m_Reg.group<TransformComponent>(entt::get<SpriteRendererComponent>);
		for (auto entity : group)
		{
			auto& [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);

			Renderer2D::DrawQuad(transform, sprite.Color);
		}
	}
}

/* DEMO code to be deleted removed from Scene::Scene()
		struct MeshComponent
		{
			bool Data;
			MeshComponent() = default;
		};
		struct TransformComponent
		{
			glm::mat4 Transform;

			TransformComponent() = default;
			TransformComponent(const TransformComponent&) = default;
			TransformComponent(const glm::mat4& transform)
				:Transform(transform) {}

			operator glm::mat4&() { return Transform; }
			operator const glm::mat4&() const { return Transform; }
		};

		//TransformComponent transform;
		//DoMath(transform);

		entt::entity entity = m_Reg.create(); // create entity
		auto& transform = m_Reg.emplace<TransformComponent>(entity, glm::mat4(1.0f)); // add a transform matrix to compoent

		m_Reg.on_construct<TransformComponent>().connect<&OnTransformConstuct>();

		if (m_Reg.has<TransformComponent>(entity))
		{
			auto& transform = m_Reg.get<TransformComponent>(entity);
			transform = glm::mat4(1.0f); // make changes to the transform
			//m_Reg.remove(entity); // delete a entity
		}

		// go through all enititys with a component type eg TransformComponent
		auto view = m_Reg.view<TransformComponent>();
		for (auto entity : view)
		{
			TransformComponent& transform = m_Reg.get<TransformComponent>(entity);
			// do something with the transform
		}

		auto group = m_Reg.group<TransformComponent>(entt::get<MeshComponent>);
		for (auto entity : group)
		{
			auto&[transform, mesh] = group.get<TransformComponent, MeshComponent>(entity);
			// do something with the transform or the mesh
			//Renderer::Submit(mesh, transform);
		}
		*/