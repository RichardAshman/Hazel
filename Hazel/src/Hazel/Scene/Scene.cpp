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
		// Update scripts
		{
			m_Registry.view<NativeScriptComponent>().each([=](auto entity, auto& nsc)
			{
				// TODO: might not need this if(!nsc.Instance) when there is a concept of run time.
				if (!nsc.Instance)
				{
					nsc.InstantiateFunction();
					nsc.Instance->m_Entity = { entity, this };

					if (nsc.OnCreateFunction)
					{
						nsc.OnCreateFunction(nsc.Instance);
					}
				}

				if (nsc.OnCreateFunction)
				{
					nsc.OnUpdateFunction(nsc.Instance, ts);
				}
			});
		}

		// Render 2D
		Camera* mainCamera = nullptr;
		glm::mat4* cameraTransform = nullptr;
		{
			auto view = m_Registry.view<TransformComponent, CameraComponent>();
			for (auto entity : view)
			{
				auto& [transform, camera] = view.get<TransformComponent, CameraComponent>(entity);

				if (camera.Primary)
				{
					mainCamera = &camera.Camera;
					cameraTransform = &transform.Transform;
					break;
				}
			}
		}
		if (mainCamera)
		{
			Renderer2D::BeginScene(mainCamera->GetProjection(), *cameraTransform);

			auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
			for (auto entity : group)
			{
				auto& [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);
				//if (sprite.Texture)
				//{
				//	Renderer2D::DrawQuad(transform, sprite.Texture, 1.0f, sprite.Color);
				//}
				//else
				//{
					Renderer2D::DrawQuad(transform, sprite.Color);
				//}
			}
			Renderer2D::EndScene();
		}
	}
	void Scene::OnViewportResize(uint32_t width, uint32_t height)
	{
		m_ViewportWidth = width;
		m_ViewportHeight = height;

		// Resize our non-FixedAspectRatio cameras
		auto view = m_Registry.view<CameraComponent>();
		for (auto entity : view)
		{
			auto& cameraComponent = view.get<CameraComponent>(entity);
			if (!cameraComponent.FixedAspectRatio)
			{
				cameraComponent.Camera.SetViewportSize(width, height);
			}
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