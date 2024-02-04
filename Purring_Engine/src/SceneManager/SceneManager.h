/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     SceneManager.h
 \date     26-11-2023

 \author               Brandon Ho Jun Jie
 \par      email:      brandonjunjie.ho@digipen.edu

 \brief    This file contains the implementation of functions and classes required for
           managing scenes.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/
/*                                                                                                          includes
--------------------------------------------------------------------------------------------------------------------- */
#include "Singleton.h"

namespace PE
{
    class SceneManager : public Singleton<SceneManager>
    {
    public:
        friend class Singleton<SceneManager>;
        // ----- Public Variables ----- //

    public:
        // ----- Public methods ----- //
        
        /*!***********************************************************************************
        \brief Sets the start scene to be loaded when the engine starts.

        \param[in] r_sceneName Name of the scene to be loaded.
        *************************************************************************************/
        void SetStartScene(std::string const& r_sceneName);

        /*!***********************************************************************************
        \brief Sets the active scene.

        \param[in] r_sceneName Name of the scene to set active.
        *************************************************************************************/
        void SetActiveScene(std::string const& r_sceneName);

        /*!***********************************************************************************
        \brief Loads a scene.

        \param[in] r_sceneName Name of the scene to be loaded.
        *************************************************************************************/
        void LoadScene(std::string const& r_sceneName);

        /*!***********************************************************************************
        \brief Loads a scene from a absolute path.

        \param[in] r_scenePath Path of the scene to be loaded.
        *************************************************************************************/
        void LoadSceneFromPath(std::string const& r_scenePath);

        /*!***********************************************************************************
        \brief Restarts the scene with reloading resources.

        \param[in] r_scenePath Path of the scene to be loaded.
        *************************************************************************************/
        void RestartScene(std::string const& r_scenePath);

        /*!***********************************************************************************
        \brief Over load of Restart Scene without a filepath
        *************************************************************************************/
        void RestartScene();

        /*!***********************************************************************************
        \brief Deletes all objects in the scene.
        *************************************************************************************/
        void DeleteObjects();

        /*!***********************************************************************************
        \brief Creates a default scene.
        *************************************************************************************/
        void CreateDefaultScene();

        /*!***********************************************************************************
        \brief Gets the name of the start scene.
        *************************************************************************************/
        inline std::string const& GetStartScene() const { return m_startScene; }

        /*!***********************************************************************************
        \brief Gets the name of the active scene.
        *************************************************************************************/
        inline std::string const& GetActiveScene() const { return m_activeScene; }

    private:
        /*!***********************************************************************************
        \brief Default constructor.
        *************************************************************************************/
        SceneManager();

        std::string m_sceneDirectory;
        std::string m_startScene;
        std::string m_activeScene{ "DefaultScene.json" };
    };
}
