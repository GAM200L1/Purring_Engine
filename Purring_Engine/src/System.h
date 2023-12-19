/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     System.h
 \creation date:       30-08-2023
 \last updated:        16-09-2023
 \author:              Brandon HO Jun Jie

 \par      email:      brandonjunjie.ho@digipen.edu

 \brief    Defines an interface for systems in the Purring Engine.
		   Each system should implement initialization, update, and destruction logic.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/
#pragma once
#include <string>
#include <vector>
#include "Singleton.h"
/*                                                                                                          includes
--------------------------------------------------------------------------------------------------------------------- */

#define GETINPUTSYSTEM() PE::SystemManager::GetInstance().GetSystem<PE::InputSystem, PE::SystemID::INPUT>()
#define GETGUISYSTEM() PE::SystemManager::GetInstance().GetSystem<PE::GUISystem, PE::SystemID::GUISYSTEM>()
#define GETLOGICSYSTEM() PE::SystemManager::GetInstance().GetSystem<PE::LogicSystem, PE::SystemID::LOGIC>()
#define GETPHYSICSMANAGER() PE::SystemManager::GetInstance().GetSystem<PE::PhysicsManager, PE::SystemID::PHYSICS>()
#define GETCOLLISIONMANAGER() PE::SystemManager::GetInstance().GetSystem<PE::CollisionManager, PE::SystemID::COLLISION>()
#define GETANIMATIONMANAGER() PE::SystemManager::GetInstance().GetSystem<PE::AnimationManager, PE::SystemID::ANIMATION>()
#define GETCAMERAMANAGER() PE::SystemManager::GetInstance().GetSystem<PE::Graphics::CameraManager, PE::SystemID::CAMERA>()
#define GETRENDERERMANAGER() PE::SystemManager::GetInstance().GetSystem<PE::Graphics::RendererManager, PE::SystemID::GRAPHICS>()

namespace PE
{
	constexpr auto TotalSystems = 8;
	/*!***********************************************************************************
	 \brief Enumeration for identifying different subsystems.
	*************************************************************************************/
	enum SystemID
	{
		INPUT = 0,
		GUISYSTEM,
		LOGIC,
		PHYSICS,
		COLLISION,
		ANIMATION,
		CAMERA,
		GRAPHICS,
		SYSTEMCOUNT
	};

	class System
	{
	public:
		/*!***********************************************************************************
		 \brief     Virtual destructor for proper cleanup of derived systems.
		*************************************************************************************/
		virtual ~System() {}

		/*!***********************************************************************************
		 \brief     Initialize the system. Should be called once before any updates.
		*************************************************************************************/
		virtual void InitializeSystem() {}

		/*!***********************************************************************************
		 \brief     Update the system each frame.
		 \param     deltaTime Time passed since the last frame, in seconds.
		*************************************************************************************/
		virtual void UpdateSystem(float deltaTime) = 0;

		/*!***********************************************************************************
		 \brief     Clean up the system resources. Should be called once after all updates.
		*************************************************************************************/
		virtual void DestroySystem() = 0;

		/*!***********************************************************************************
		 \brief     Get the system's name, useful for debugging and identification.
		 \return    std::string The name of the system.
		*************************************************************************************/
		virtual std::string GetName() = 0;
	};

	class SystemManager : public Singleton<SystemManager>
	{
	public:
		friend class Singleton<SystemManager>;

		void AddSystem(System* system)
		{
			m_systemList.push_back(system);
		}

		template<typename T, SystemID systemID>
		T const* GetSystem()
		{
			return dynamic_cast<T*>(m_systemList[systemID]);
		}

	private:

		/*!***********************************************************************************
		 \brief Constructor for the SystemManager class.
		*************************************************************************************/
		SystemManager() {}

		~SystemManager() { m_systemList.clear(); }

		std::vector<System*> m_systemList;
	};
}
