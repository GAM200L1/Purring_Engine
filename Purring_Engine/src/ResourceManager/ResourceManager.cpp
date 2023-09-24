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

    std::shared_ptr<Graphics::Texture> ResourceManager::GetTexture(std::string const& r_name)
    {
        return Textures[r_name];
    }

    void ResourceManager::UnloadResources()
    {
        ShaderPrograms.clear();
        Textures.clear();
    }
}
