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
#include "Data/SerializationManager.h"
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

        if (!Fonts[r_key]->Initialize(r_filePath))
        {
            std::cout << "Fail to load font" << r_filePath << std::endl;
            Fonts.erase(r_key);
        }
    }

    void ResourceManager::LoadAnimationFromFile(std::string const& r_key, std::string const& r_filePath)
    {
        //std::string animationID = AnimationManager::CreateAnimation(r_key);
        //nlohmann::json animationJson;

        //// Read the JSON file
        //std::ifstream i(r_filePath);
        //if (i.is_open() && nlohmann::json::accept(i))
        //{
        //    i >> animationJson;
        //    i.close();

        //    try
        //    {
        //        auto animation = std::make_shared<Animation>(Animation::Deserialize(animationJson));

        //        // If deserialization successful, add it to the Animations map
        //        Animations[r_key] = animation;
        //    }
        //    catch (const std::exception& e)
        //    {
        //        std::cout << "Couldn't create animation " << r_filePath << ". Error: " << e.what() << std::endl;
        //        Animations.erase(r_key);
        //    }
        //}
        //else
        //{
        //    std::cout << "Couldn't open animation file " << r_filePath << std::endl;                // Handle file not opening correctly
        //    Animations.erase(r_key);
        //}
    }

    std::shared_ptr<Graphics::Texture> ResourceManager::GetTexture(std::string const& r_name)
    {
        if (Textures.find(r_name) != Textures.end())
        {
            return Textures[r_name];
        }
        else
        {
            // return default texture
			std::cout << "Texture " << r_name << " not found" << std::endl;
			return nullptr;
		}
    }

    std::shared_ptr<Animation> ResourceManager::GetAnimation(std::string const& r_name)
    {
        if (Animations.find(r_name) != Animations.end())
        {
			return Animations[r_name];
		}
        else
        {
			// return default animation
			std::cout << "Animation " << r_name << " not found" << std::endl;
			return nullptr;
		}
	}

    void ResourceManager::UnloadResources()
    {
        ShaderPrograms.clear();
        Textures.clear();
        Sounds.clear();
        Fonts.clear();
    }

}
