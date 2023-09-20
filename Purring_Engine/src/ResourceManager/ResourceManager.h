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
            Currently only supports shader and texture loading.
 
 
 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/
/*                                                                                                          includes
--------------------------------------------------------------------------------------------------------------------- */
#include "prpch.h"
#include "Graphics/ShaderProgram.h"
#include "Graphics/Texture.h"

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
        static std::map<std::string, std::shared_ptr<Graphics::ShaderProgram>> ShaderPrograms;
        static std::map<std::string, std::shared_ptr<Graphics::Texture>> Textures;  
        // Icons?

        // Delete any copy functions
        // don't need copy constructor or assignment
        // Another ResourceManager should not exist in run
        ResourceManager(ResourceManager const& r_cpyRM) = delete;
        ResourceManager operator=(ResourceManager const& r_cpyRM) = delete;

        // Gets the instance of this class (i need to check why i need this;;)
        static ResourceManager* GetInstance();
        static void DeleteInstance();

        // Unloads all resources that were previously loaded
        static void UnloadResources();

    public:
        // ----- Public methods ----- //
        // ----- Get/Load Resource/Asset Functions ----- //
        
        /*!***********************************************************************************
            \brief Loads shaders from file, compile them and inserts into ShaderProgram map
                   container.

            \param[in] r_key Name of shader program.
            \param[in] r_vertexShaderPath File path of vertex shader.
            \param[in] r_fragmentShaderPath File path of fragment shader.
        *************************************************************************************/
        static void LoadShadersFromFile(std::string const& r_key, std::string const& r_vertexShaderString,
            std::string const& r_fragmentShaderString);

        // wip
        //static PE::Graphics::ShaderProgram* GetShaderProgram(std::string const& r_key,
        //    std::string const& r_vertexShaderString,
        //    std::string const& r_fragmentShaderString);

        /*!***********************************************************************************
            \brief Loads texture from file and inserts into Textures map container.

            \param[in] r_name Name of texture.
            \param[in] r_filePath File path of texture.
        *************************************************************************************/
        static void LoadTextureFromFile(std::string const& r_name, std::string const& r_filePath);

        /*!***********************************************************************************
            \brief Gets the texture object store in the resource manager.

            \param[in] r_name Name of texture.

            \return Texture object in map.
        *************************************************************************************/
        static std::shared_ptr<Graphics::Texture> GetTexture(std::string const& r_name);

    private:

        // pointer to the instance of this class
        static ResourceManager* p_instance;
        // constructor
        ResourceManager() {}
        ~ResourceManager() {}
    };
}

