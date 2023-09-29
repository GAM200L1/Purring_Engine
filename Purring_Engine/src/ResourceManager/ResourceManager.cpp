/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     ResourceManager.cpp
 \date     25-08-2023

 \author               Liew Yeni
 \par      email:      yeni.l\@digipen.edu
 \par      code %:     25%
 \par      changes:    Original Author

 \co-author            Brandon Ho Jun Jie
 \par      email:      brandonjunjie.ho\@digipen.edu
 \par      code %:     70%
 \par      changes:    29-09-2023
                       Loading Textures, Audio, Font, Shader programs

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
            Currently only supports shader and texture loading.


 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/
/*                                                                                                          includes
--------------------------------------------------------------------------------------------------------------------- */
#include "prpch.h"
#include "ResourceManager.h"

namespace PE
{
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

    void ResourceManager::LoadFontFromFile(std::string const& r_key, std::string const& r_filePath)
    {
        Fonts[r_key] = std::make_shared<Font>();

        if (!Fonts[r_key]->Load(r_filePath))
        {
            Fonts.erase(r_key);
        }
    }

    std::shared_ptr<Graphics::Texture> ResourceManager::GetTexture(std::string const& r_name)
    {
        return Textures[r_name];
    }

    void ResourceManager::UnloadResources()
    {
        ShaderPrograms.clear();
        Textures.clear();
        Sounds.clear();
        Fonts.clear();
    }

}
