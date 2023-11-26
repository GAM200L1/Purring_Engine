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
            \brief Loads shaders from file, compile them and inserts into ShaderProgram map
                   container.

            \param[in] r_key Name of shader program.
            \param[in] r_vertexShaderPath File path of vertex shader.
            \param[in] r_fragmentShaderPath File path of fragment shader.
        *************************************************************************************/
        void SetStartScene(std::string const& r_sceneName);

        void SetCurrentScene(std::string const& r_sceneName);

        void LoadCurrentScene();

        inline std::string const& GetStartScene() const { return m_startScene; }

        std::string const& GetCurrentScene() const { return m_currentScene; }

    private:
        // constructor
        SceneManager();

        std::string m_sceneDirectory;
        std::string m_startScene;
        std::string m_currentScene;
    };
}
