/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     ResourceManager.cpp
 \date     25-08-2023
 
 \author               Liew Yeni
 \par      email:      yeni.l\@digipen.edu
 \par      code %:     55%
 \par      changes:    Original Author

 \co-author            Brandon Ho Jun Jie
 \par      email:      brandonjunjie.ho\@digipen.edu
 \par      code %:     40%
 \par      changes:    16-09-2023
                       Texture loading, Shader loading
 
 \co-author            Foong Jun Wei
 \par      email:      f.junwei\@digipen.edu
 \par      code %:     5%
 \par      changes:    16-09-2023
                       Bug fixes, updated file header, minor changes to namespacing (PE) 

                       Bug fixes:
                       Static Variables were not re-declared at the top of the
                       file, causing linker errors.

                       Lifetime of newShader ended (auto destruct and stuff)
                       causing the shader program to not exist when called
                       later on in the shader program.
 
 \brief     This file contains the implementation for the resource manager.
            Currently only supports shader programs.
 
 
 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/
/*                                                                                                          includes
--------------------------------------------------------------------------------------------------------------------- */
#include "prpch.h"
#include "ResourceManager.h"

namespace PE
{
    /*!***********************************************************************************
     \brief     Loads and compiles shaders from files and stores them in a resource map.

     \tparam T  This function does not use a template.
     \param[in] r_key                   The key used to identify the shader program in the resource map.
     \param[in] r_vertexShaderPath      The file path to the vertex shader.
     \param[in] r_fragmentShaderPath    The file path to the fragment shader.
     \return    void                    This function does not return a value but updates the ShaderPrograms map.
                                        If shaders fail to compile, the key is deleted from the map.
    *************************************************************************************/
    void ResourceManager::LoadShadersFromFile(std::string const& r_key, std::string const& r_vertexShaderPath,
                             std::string const& r_fragmentShaderPath)
    {
       ShaderPrograms[r_key] = std::make_shared<Graphics::ShaderProgram>();
        
        if (!ShaderPrograms[r_key]->LoadAndCompileShadersFromFile(r_vertexShaderPath, r_fragmentShaderPath))
        {
            // fail to compile, delete key
            ShaderPrograms.erase(r_key);
        }
    }

    // wip
    //Graphics::ShaderProgram* ResourceManager::GetShaderProgram(std::string const& r_key, std::string const& r_vertexShaderString, std::string const& r_fragmentShaderString)
    //{
    //    //if (m_shaderPrograms.find(r_key) == m_shaderPrograms.end())
    //    //{
    //    //    // Graphics::ShaderProgram newShader{};
    //    //    Graphics::ShaderProgram* newShader = new Graphics::ShaderProgram();
    //    //    if (newShader->CompileLinkValidateProgram(r_vertexShaderString, r_fragmentShaderString))
    //    //        m_shaderPrograms[r_key] = newShader;
    //    //    else
    //    //        throw;
    //    //}

    //    //return m_shaderPrograms[r_key];
    //}

    /*!***********************************************************************************
     \brief     Loads a texture from a file and stores it in the Textures resource map.

     \tparam T  This function does not use a template.
     \param[in] r_name            The key used to identify the texture in the resource map.
     \param[in] r_filePath        The file path to the texture.
     \return    void              This function does not return a value but updates the Textures map.
                                  If texture creation fails, an error message is printed and the key is deleted from the map.
    *************************************************************************************/
    void ResourceManager::LoadTextureFromFile(std::string const& r_name, std::string const& r_filePath)
    {
        Textures[r_name] = std::make_shared<Graphics::Texture>();
        if (!Textures[r_name]->CreateTexture(r_filePath))
        {
            // fail to create texture, delete key
            std::cout << "Couldn't create texture " << r_filePath << std::endl;
            Textures.erase(r_name);
        }
    }

    /*!***********************************************************************************
     \brief     Loads an audio file and stores it in the Sounds resource map.

     \tparam T  This function does not use a template.
     \param[in] r_key             The key used to identify the audio in the resource map.
     \param[in] r_filePath        The file path to the audio file.
     \return    void              This function does not return a value but updates the Sounds map.
                                  If audio loading fails, an error message is printed and the key is deleted from the map.
    *************************************************************************************/
    void ResourceManager::LoadAudioFromFile(std::string const& r_key, std::string const& r_filePath)
    {
        Sounds[r_key] = std::make_shared<AudioManager::Audio>();

        if (AudioManager::GetInstance().GetFMODSystem() == nullptr)
        {
            std::cout << "NO SYSTEM";
        }

        if (!Sounds[r_key]->LoadSound(r_filePath, AudioManager::GetInstance().GetFMODSystem()))
        {
            std::cout << "Fail to load sound" << r_filePath << std::endl;
            // fail to load sound, delete key
            Sounds.erase(r_key);
        }
    }



    /*!***********************************************************************************
     \brief     Retrieves a texture based on its key from the Textures resource map.

     \tparam T  This function does not use a template.
     \param[in] r_name            The key used to identify the texture in the resource map.
     \return    std::shared_ptr<Graphics::Texture>  Returns a shared pointer to the texture object associated with the given key.
    *************************************************************************************/
    std::shared_ptr<Graphics::Texture> ResourceManager::GetTexture(std::string const& r_name)
    {
        return Textures[r_name];
    }



    /*!***********************************************************************************
     \brief     Unloads all resources, clearing the resource maps.

     \tparam T  This function does not use a template.
     \return    void  This function does not return a value but clears the ShaderPrograms and Textures maps.
    *************************************************************************************/
    void ResourceManager::UnloadResources()
    {
        ShaderPrograms.clear();
        Textures.clear();
    }

}
