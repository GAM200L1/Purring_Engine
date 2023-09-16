/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     ResourceManager.h
 \date     25-08-2023
 
 \author               Liew Yeni
 \par      email:      yeni.l\@digipen.edu
 \par      code %:     98%
 \par      changes:    Original Author
 
 \co-author            Foong Jun Wei
 \par      email:      f.junwei\@digipen.edu
 \par      code %:     2%
 \par      changes:    16-09-2023
                       Updated file header & minor changes to namespacing (PE)

 \brief     This file contains the implementation for the resource manager.
            Currently only supports shader programs.
 
 
 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/
#pragma once

/*                                                                                                          includes
--------------------------------------------------------------------------------------------------------------------- */
#include "prpch.h"
#include "../Graphics/ShaderProgram.h"

//class Resource
//{
//public:
//    
//    Resource(); // don't allow no specification of the file path
//    //Resource(std::string const& r_filepath);
//    ~Resource() = default;
//
//protected:
//    unsigned int m_resourceID;
//    std::string m_filepath;
//};

// should namespace this under the engine
// setting as singleton class - think theres only need for one instance of this
namespace PE
{
    class ResourceManager
    {
    public:
        // ----- Public Variables ----- //
        static std::map<std::string, Graphics::ShaderProgram*> m_shaderPrograms;
        

        // Delete any copy functions
        // don't need copy constructor or assignment
        // Another ResourceManager should not exist in run
        ResourceManager(ResourceManager const& r_cpyRM) = delete;
        ResourceManager operator=(ResourceManager const& r_cpyRM) = delete;

        // Gets the instance of this class (i need to check why i need this;;)
        static ResourceManager* GetInstance();

        // Unloads all resources that were previously loaded
        static void UnloadResources();

    public:
        // ----- Public methods ----- //
        // ----- Get/Load Resource/Asset Functions ----- //
        // to get an element from the map in this class
        static PE::Graphics::ShaderProgram* GetShaderProgram(std::string const& r_key,
            std::string const& r_vertexShaderString,
            std::string const& r_fragmentShaderString);

    private:

        // pointer to the instance of this class
        static ResourceManager* p_instance;
        //constructor
        ResourceManager() {}
    };
}

