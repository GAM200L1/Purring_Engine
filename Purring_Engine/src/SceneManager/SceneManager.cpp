/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     SceneManager.cpp
 \date     26-11-2023

 \author               Brandon Ho Jun Jie
 \par      email:      brandonjunjie.ho@digipen.edu

 \brief    This file contains the implementation of functions and classes required for
           managing scenes.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/
/*                                                                                                          includes
--------------------------------------------------------------------------------------------------------------------- */
#include "prpch.h"
#include "SceneManager.h"
#include "Data/SerializationManager.h"
#include "Logging/Logger.h"

extern Logger engine_logger;

namespace PE
{
    SceneManager::SceneManager()
        : m_sceneDirectory("../Assets/Scenes/")
    {

	}

    void SceneManager::SetStartScene(std::string const& r_sceneName)
    {
		m_startScene = r_sceneName;
        m_currentScene = r_sceneName;
	}

    void SceneManager::SetCurrentScene(std::string const& r_sceneName)
    {
		m_currentScene = r_sceneName;
    }

    void SceneManager::LoadCurrentScene()
    {
		SerializationManager serializationManager;
		serializationManager.DeleteAllObjectAndLoadAllEntitiesFromFile(m_sceneDirectory + m_currentScene);
	}
}
