#include "hzpch.h"
#include "Components.h"
#include "Scene.h"

#include "Hazel/Renderer/Renderer2D.h"

#include <glm/glm.hpp>

#include "Entity.h"

namespace Hazel
{
	Scene::Scene()
	{
	}
	Scene::~Scene()
	{
	}
	Entity Scene::CreateEntity(const std::string& name)
	{
		Entity entity = { m_Registry.create(), this};
		entity.AddComponent<TransformComponent>();
		auto& tag = entity.AddComponent<TagComponent>();
		tag.Tag = name.empty() ? "Entity" : name;
		return entity;
	}
	void Scene::OnUpdate(Timestep ts)
	{
		auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
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

		entt::entity entity = m_Registry.create(); // create entity
		auto& transform = m_Registry.emplace<TransformComponent>(entity, glm::mat4(1.0f)); // add a transform matrix to compoent

		m_Registry.on_construct<TransformComponent>().connect<&OnTransformConstuct>();

		if (m_Registry.has<TransformComponent>(entity))
		{
			auto& transform = m_Registry.get<TransformComponent>(entity);
			transform = glm::mat4(1.0f); // make changes to the transform
			//m_Registry.remove(entity); // delete a entity
		}

		// go through all enititys with a component type eg TransformComponent
		auto view = m_Registry.view<TransformComponent>();
		for (auto entity : view)
		{
			TransformComponent& transform = m_Registry.get<TransformComponent>(entity);
			// do something with the transform
		}

		auto group = m_Registry.group<TransformComponent>(entt::get<MeshComponent>);
		for (auto entity : group)
		{
			auto&[transform, mesh] = group.get<TransformComponent, MeshComponent>(entity);
			// do something with the transform or the mesh
			//Renderer::Submit(mesh, transform);
		}
		*/