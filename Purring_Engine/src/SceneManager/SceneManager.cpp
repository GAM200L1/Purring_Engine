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
#include "ResourceManager/ResourceManager.h"
#include "Hierarchy/HierarchyManager.h"
#include "Logic/LogicSystem.h"
#include "Graphics/CameraManager.h"

extern Logger engine_logger;

namespace PE
{
    SceneManager::SceneManager()
        : m_sceneDirectory("../Assets/Scenes/")
    {

	}

    void SceneManager::CreateDefaultScene()
    {
        SetActiveScene("DefaultScene.json");
        LoadSceneFromPath("../Assets/Defaults/DefaultScene.json");
    }

    void SceneManager::SetStartScene(std::string const& r_sceneName)
    {
		m_startScene = r_sceneName;
	}

    void SceneManager::SetActiveScene(std::string const& r_sceneName)
    {
		m_activeScene = r_sceneName;
    }

    void SceneManager::LoadSceneToLoad()
    {
        // check if scene is already loaded
        if (m_sceneToLoad == m_activeScene)
        {
			RestartScene(m_activeScene);
            m_loadingScene = false;
			return;
		}

        // set active scene
        SetActiveScene(m_sceneToLoad);

		// load scene from path
		LoadSceneFromPath(m_sceneDirectory + m_sceneToLoad);
        m_loadingScene = false;
	}

    void SceneManager::LoadSceneFromPath(std::string const& r_scenePath)
    {
        // delete all objects
        DeleteObjects();

        // unload all resources
        ResourceManager::GetInstance().UnloadResources();

        std::filesystem::path filepath = r_scenePath;

        if (!std::filesystem::exists(filepath))
        {
            std::cerr << "File does not exist: " << filepath << std::endl;
            return;
        }

        std::ifstream inFile(filepath);
        if (inFile)
        {
            SerializationManager serializationManager;
            nlohmann::json allEntitiesJson;
            inFile >> allEntitiesJson;
            serializationManager.DeserializeAllEntities(allEntitiesJson);
            inFile.close();
        }
        else
        {
            std::cerr << "Could not open the file for reading: " << filepath << std::endl;
        }

        // load all resources
        ResourceManager::GetInstance().LoadAllResources();
    }

    void SceneManager::LoadScene(std::string const& r_scenePath)
    {
		m_sceneToLoad = r_scenePath;
		m_loadingScene = true;
	}

    void SceneManager::RestartScene(std::string const& r_scenePath)
    {
        // delete all objects
        DeleteObjects();

        // restart scene should be loading current scene, but for now just given scene path for editor savestate
        std::filesystem::path filepath = m_sceneDirectory + r_scenePath;

        if (!std::filesystem::exists(filepath))
        {
            std::cerr << "File does not exist: " << filepath << std::endl;
            return;
        }

        std::ifstream inFile(filepath);
        if (inFile)
        {
            SerializationManager serializationManager;
            nlohmann::json allEntitiesJson;
            inFile >> allEntitiesJson;
            serializationManager.DeserializeAllEntities(allEntitiesJson);
            inFile.close();
        }
        else
        {
            std::cerr << "Could not open the file for reading: " << filepath << std::endl;
        }

        m_restartingScene = false;
    }

    void SceneManager::RestartScene()
    {
        m_restartingScene = true;
    }

    void SceneManager::DeleteObjects()
    {
        // delete all objects
        std::vector<EntityID> temp = EntityManager::GetInstance().GetEntitiesInPool(ALL);

        for (auto n : temp)
        {
            if (n != Graphics::CameraManager::GetUiCameraId())
            {
                LogicSystem::DeleteScriptData(n);
                EntityManager::GetInstance().RemoveEntity(n);
            }
        }
        Hierarchy::GetInstance().Update();
    }
}
