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

	private:
		entt::registry m_Registry; //m_Registry. Storage of entity IDs (think context containing entities)

		friend class Entity;
	};
}