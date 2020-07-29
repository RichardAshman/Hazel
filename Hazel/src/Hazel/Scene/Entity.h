#pragma once

#include "Scene.h"
#include "entt.hpp"

namespace Hazel
{
	class Entity
	{
public:
	Entity() = default;
	Entity(entt::entity handle, Scene* scene);
	Entity(const Entity& other) = default;

	template<typename T, typename... Args>
	T& AddComponent(Args&&...args)
	{
		HZ_CORE_ASSERT(!HasComponent<T>(), "Entity already has component!");
		return m_Scene->m_Registry.emplace<T>(m_EntityHandle, std::forward<Args>(args)...);
	}

	template<typename T>
	T& GetComponent()
	{
		HZ_CORE_ASSERT(HasComponent<T>(), "Entity does not have a component of that type!");
		return m_Scene->m_Registry.get<T>(m_EntityHandle);
	}

	template<typename T>
	bool HasComponent()
	{
		return m_Scene->m_Registry.has<T>(m_EntityHandle);
	}

	template<typename T >
	void RemoveComponent()
	{
		HZ_CORE_ASSERT(HasComponent<T>(), "Entity does not have a component of that type!");
		m_Scene->m_Registry.remove<T>(m_EntityHandle);
	}
	
	operator bool() const 
	{
		return /*(uint32_t)*/m_EntityHandle != entt::null/*(uint32_t)0*/;
	}

private:
	entt::entity m_EntityHandle{1 };

	Scene* m_Scene = nullptr; // make sure to check if it points to anything before use
	//TODO: change to something like weakref?
	//std::weak_ref<Scene> m_Scene;
	};

}