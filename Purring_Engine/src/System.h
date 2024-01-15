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
		   
		   Defines an interface to request access to a system.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/
#pragma once
#include <string>
#include <vector>
#include "Singleton.h"
/*                                                                                                          includes
--------------------------------------------------------------------------------------------------------------------- */

/*!***********************************************************************************
 \brief Retrieves Input system instance to use its functions
 
*************************************************************************************/
#define GETINPUTSYSTEM() PE::SystemManager::GetInstance().GetSystem<PE::InputSystem, PE::SystemID::INPUT>()

/*!***********************************************************************************
 \brief Retrieves GUI system instance to use its functions
 
*************************************************************************************/
#define GETGUISYSTEM() PE::SystemManager::GetInstance().GetSystem<PE::GUISystem, PE::SystemID::GUISYSTEM>()

/*!***********************************************************************************
 \brief Retrieves Logic system instance to use its functions
 
*************************************************************************************/
#define GETLOGICSYSTEM() PE::SystemManager::GetInstance().GetSystem<PE::LogicSystem, PE::SystemID::LOGIC>()

/*!***********************************************************************************
 \brief Retrieves Physics system instance to use its functions
 
*************************************************************************************/
#define GETPHYSICSMANAGER() PE::SystemManager::GetInstance().GetSystem<PE::PhysicsManager, PE::SystemID::PHYSICS>()

/*!***********************************************************************************
 \brief Retrieves Collision system instance to use its functions
 
*************************************************************************************/
#define GETCOLLISIONMANAGER() PE::SystemManager::GetInstance().GetSystem<PE::CollisionManager, PE::SystemID::COLLISION>()

/*!***********************************************************************************
 \brief Retrieves Animation system instance to use its functions
 
*************************************************************************************/
#define GETANIMATIONMANAGER() PE::SystemManager::GetInstance().GetSystem<PE::AnimationManager, PE::SystemID::ANIMATION>()

/*!***********************************************************************************
 \brief Retrieves Camera system instance to use its functions
 
*************************************************************************************/
#define GETCAMERAMANAGER() PE::SystemManager::GetInstance().GetSystem<PE::Graphics::CameraManager, PE::SystemID::CAMERA>()

/*!***********************************************************************************
 \brief Retrieves Visual Effects system instance to use its functions

*************************************************************************************/
#define GETVISUALEFFECTSMANAGER() PE::SystemManager::GetInstance().GetSystem<PE::VisualEffectsManager, PE::SystemID::VISUALEFFECTS>()

/*!***********************************************************************************
 \brief Retrieves Renderer system instance to use its functions
 
*************************************************************************************/
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
		VISUALEFFECTS,
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

		/*!***********************************************************************************
		 \brief     Add system to the system list in the SystemManager.

		 \param     system The system to be added to the system list.
		*************************************************************************************/
		void AddSystem(System* system)
		{
			m_systemList.push_back(system);
		}

		/*!***********************************************************************************
		\brief     Get system from the system list in the SystemManager.

		\tparam    T The type of system to be retrieved from the system list.
		\tparam    systemID The ID of the system to be retrieved from the system list.

		\return    T const* The system to be retrieved from the system list.
		*************************************************************************************/
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

		/*!***********************************************************************************
		 \brief Destructor for the SystemManager class.
		*************************************************************************************/
		~SystemManager() { m_systemList.clear(); }

		std::vector<System*> m_systemList;
	};
}
