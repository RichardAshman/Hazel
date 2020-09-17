#pragma once
#include "entt.hpp"
#include "Hazel/Core/Timestep.h"

namespace Hazel
{
	class Entity;
	class Scene
	{
	public :
		Scene();
		~Scene();

		Entity CreateEntity(const std::string& name = std::string());

		//TEMP
		//entt::registry& GetReg() { return m_Registry; }



		void OnUpdate(Timestep ts);
		void OnViewportResize(uint32_t width, uint32_t height);
	private:
		entt::registry m_Registry; //m_Registry. Storage of entity IDs (think context containing entities)
		uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;
		friend class Entity;
		friend class SceneHierarchyPanel;
	};
}