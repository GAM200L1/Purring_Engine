/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     ResourceManager.h
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
#include "Singleton.h"
#include "Graphics/ShaderProgram.h"
#include "Graphics/Texture.h"
#include "AudioManager/AudioManager.h"
#include "Graphics/Text.h"
#include "Animation/Animation.h"

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

namespace PE
{
    class ResourceManager : public Singleton<ResourceManager>
    {
    public:
        friend class Singleton<ResourceManager>;
        // ----- Public Variables ----- //
        std::map<std::string, std::shared_ptr<Graphics::ShaderProgram>> ShaderPrograms;
        std::map<std::string, std::shared_ptr<Graphics::Texture>> Textures;
        std::map<std::string, std::shared_ptr<AudioManager::Audio>> Sounds;
        std::map<std::string, std::shared_ptr<Font>> Fonts;
        std::map<std::string, std::shared_ptr<Graphics::Texture>> Icons;
        std::map<std::string, std::shared_ptr<Animation>> Animations;

        // Unloads all resources that were previously loaded
        void UnloadResources();

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
        void LoadShadersFromFile(std::string const& r_key, std::string const& r_vertexShaderString,
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
        bool LoadTextureFromFile(std::string const& r_name, std::string const& r_filePath);

        /*!***********************************************************************************
            \brief Loads icon from file and inserts into Icons map container.

            \param[in] r_name Name of icon.
            \param[in] r_filePath File path of icon.
        *************************************************************************************/
        bool LoadIconFromFile(std::string const& r_name, std::string const& r_filePath);

        /*!***********************************************************************************
            \brief Loads audio from file and inserts into Sounds map container.

            \param[in] r_key Name of audio file.
            \param[in] r_filePath File path of audio.
        *************************************************************************************/
        void LoadAudioFromFile(std::string const& r_key, std::string const& r_filePath);

        /*!***********************************************************************************
            \brief Loads audio from file and inserts into Sounds map container.

            \param[in] r_key Name of audio file.
            \param[in] r_filePath File path of audio.
        *************************************************************************************/
        std::string ResourceManager::LoadDraggedAudio(std::string const& r_filePath);

        /*!***********************************************************************************
            \brief Loads font from file and inserts into Sounds map container.

            \param[in] r_key Name of font file.
            \param[in] r_filePath File path of font.
        *************************************************************************************/
        void LoadFontFromFile(std::string const& r_key, std::string const& r_filePath);

        /*!***********************************************************************************
            \brief Loads animation from file and inserts into Animations map container.

            \param[in] r_key Name of animation file.
            \param[in] r_filePath File path of animation.
        *************************************************************************************/
        void LoadAnimationFromFile(std::string const& r_key, std::string const& r_filePath);

        /*!***********************************************************************************
            \brief Gets the texture object store in the resource manager.

            \param[in] r_name Name of texture.

            \return Texture object in map.
        *************************************************************************************/
        std::shared_ptr<Graphics::Texture> GetTexture(std::string const& r_name);

        /*!***********************************************************************************
            \brief Gets the icon object store in the resource manager.

            \param[in] r_name Name of icon.

            \return Icon object in map.
        *************************************************************************************/
        std::shared_ptr<Graphics::Texture> GetIcon(std::string const& r_name);

        /*!***********************************************************************************
            \brief Gets the animation stored in the resource manager.
            
            \param[in] r_name Name of animation
                                
            \return Animation in map.
        *************************************************************************************/
        std::shared_ptr<Animation> GetAnimation(std::string const& r_name);
    private:
        // constructor
        ResourceManager()
        {

        }
    };
}
