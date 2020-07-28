#pragma once
#include "entt.hpp"
#include "Hazel/Core/Timestep.h"

namespace Hazel
{

	class Scene
	{
	public :
		Scene();
		~Scene();

		entt::entity CreateEntity();

		//TEMP
		entt::registry& GetReg() { return m_Reg; }



		void OnUpdate(Timestep ts);

	private:
		entt::registry m_Reg; //m_Registry. Storage of entity IDs (think context containing entities)


	};
}