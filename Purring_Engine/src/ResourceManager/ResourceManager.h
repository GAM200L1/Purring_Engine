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
#include "imgui.h"
#include "MetaData.h"
#include "Data/SerializationManager.h"

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
        std::map<std::string, nlohmann::json> Prefabs;

        /*!***********************************************************************************
         \brief Unloads all resources held by the resource manager.
        *************************************************************************************/
        void UnloadResources();

    public:
        // ----- Public methods ----- //
        // ----- Get/Load Resource/Asset Functions ----- //
        
        /*!***********************************************************************************
            \brief Loads default assets for use.
        *************************************************************************************/
        void LoadDefaultAssets();

        /*!***********************************************************************************
            \brief Loads shaders from file, compile them and inserts into ShaderProgram map
                   container.

            \param[in] r_key Name of shader program.
            \param[in] r_vertexShaderPath File path of vertex shader.
            \param[in] r_fragmentShaderPath File path of fragment shader.
        *************************************************************************************/
        void LoadShadersFromFile(std::string const& r_key, std::string const& r_vertexShaderString,
            std::string const& r_fragmentShaderString);

        /*!***********************************************************************************
            \brief Loads texture from file and inserts into Textures map container.

            \param[in] r_name Name of texture.
            \param[in] r_filePath File path of texture.

            \return True if texture is loaded successfully, false otherwise.
        *************************************************************************************/
        bool LoadTextureFromFile(std::string const& r_name, std::string const& r_filePath);

        /*!***********************************************************************************
            \brief Loads icon from file and inserts into Icons map container.

            \param[in] r_name Name of icon.
            \param[in] r_filePath File path of icon.

            \return True if icon is loaded successfully, false otherwise.
        *************************************************************************************/
        bool LoadIconFromFile(std::string const& r_name, std::string const& r_filePath);

        /*!***********************************************************************************
            \brief Loads audio from file and inserts into Sounds map container.

            \param[in] r_key Name of audio file.
            \param[in] r_filePath File path of audio.
        *************************************************************************************/
        bool LoadAudioFromFile(std::string const& r_key, std::string const& r_filePath);

        /*!***********************************************************************************
            \brief Loads audio from file and inserts into Sounds map container.

            \param[in] r_key Name of audio file.
            \param[in] r_filePath File path of audio.

            \return File path of audio.
        *************************************************************************************/
        std::string ResourceManager::LoadDraggedAudio(std::string const& r_filePath);

        /*!***********************************************************************************
            \brief Loads font from file and inserts into Sounds map container.

            \param[in] r_key Name of font file.
            \param[in] r_filePath File path of font.

            \return True if font is loaded successfully, false otherwise.
        *************************************************************************************/
        bool LoadFontFromFile(std::string const& r_key, std::string const& r_filePath);

        /*!***********************************************************************************
            \brief Loads animation from file and inserts into Animations map container.

            \param[in] r_key Name of animation file.
            \param[in] r_filePath File path of animation.

            \return True if animation is loaded successfully, false otherwise.
        *************************************************************************************/
        bool LoadAnimationFromFile(std::string const& r_key, std::string const& r_filePath);

        /*!***********************************************************************************
         \brief Load an entity from a prefab file, returning its ID.

         \param[in] r_filePath  The path to the file to load from.
         \param[in] useFilePath  Whether to use the file path as the entity name.

         \return ID of the entity loaded.
        *************************************************************************************/
        size_t LoadPrefabFromFile(std::string const& r_filePath, bool useFilePath = false);

        /*size_t GetPrefab(std::string const& r_name);*/

        /*!***********************************************************************************
            \brief Gets the size of a texture with the given name from the ResourceManager.

            \param[in] r_name The name of the texture to retrieve the size for.

            \return An ImVec2 containing the width and height of the texture.
        *************************************************************************************/
        ImVec2 GetTextureSize(const std::string& r_name);

        /*!***********************************************************************************
        \brief Load texture from Resource folder in Textures folder

        \param[in] r_fileName Name of texture.

        \return key of texture object
        *************************************************************************************/
        std::string LoadTexture(std::string const& r_fileName);

        /*!***********************************************************************************
        \brief Load audio from Resource folder in Audio folder

        \param[in] r_fileName Name of audio.

        \return key of audio object
        *************************************************************************************/
        std::string LoadAudio(std::string const& r_fileName);

        /*!***********************************************************************************
        \brief Load font from Resource folder in Fonts folder

        \param[in] r_fileName Name of font.

        \return key of font object
        *************************************************************************************/
        std::string LoadFont(std::string const& r_fileName);

        /*!***********************************************************************************
        \brief Load animation from Resource folder in Animation folder

        \param[in] r_fileName Name of animation.

        \return key of animation object
        *************************************************************************************/
        std::string LoadAnimation(std::string const& r_fileName);

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

        /*!***********************************************************************************
            \brief Gets the font stored in the resource manager.

            \param[in] r_name Name of font

            \return Font in map.
        *************************************************************************************/
        std::shared_ptr<Font> GetFont(std::string const& r_name);

        /*!***********************************************************************************
            \brief Gets the audio stored in the resource manager.

            \param[in] r_name Name of audio

            \return Audio in map.
        *************************************************************************************/
        std::shared_ptr<AudioManager::Audio> GetAudio(std::string const& r_name);

        /*!***********************************************************************************
        \brief Loads all the textures in texture key map.
        *************************************************************************************/
        void LoadAllTextures();

        /*!***********************************************************************************
        \brief Loads all the audio in audio key map.
        *************************************************************************************/
        void LoadAllAudio();

        /*!***********************************************************************************
        \brief Loads all the fonts in font key map.
        *************************************************************************************/
        void LoadAllFonts();

        /*!***********************************************************************************
        \brief Loads all the animations in animation key map.
        *************************************************************************************/
        void LoadAllAnimations();

        /*!***********************************************************************************
        \brief Loads all the prefabs in prefab key map.
        *************************************************************************************/
        void LoadAllPrefabs();

        /*!***********************************************************************************
        \brief Loads all the resources.
        *************************************************************************************/
        void LoadAllResources();

        /*!***********************************************************************************
        \brief Adds shader key to load.

        \param[in] r_key Filepath of texture.
        *************************************************************************************/
        void AddTextureKeyToLoad(std::string const& r_key);

        /*!***********************************************************************************
        \brief Loads all the audio in audio key map.

        \param[in] r_key Filepath of audio.
        *************************************************************************************/
        void AddAudioKeyToLoad(std::string const& r_key);

        /*!***********************************************************************************
        \brief Adds font key to load.

        \param[in] r_key Filepath of font.
        *************************************************************************************/
        void AddFontKeyToLoad(std::string const& r_key);

        /*!***********************************************************************************
        \brief Adds animation key to load.

        \param[in] r_key Filepath of animation.
        *************************************************************************************/
        void AddAnimationKeyToLoad(std::string const& r_key);

        /*!***********************************************************************************
        \brief Adds prefab key to load.

        \param[in] r_key Filepath of prefab.
        *************************************************************************************/
        void AddPrefabKeyToLoad(std::string const& r_key);
    private:

        /*!***********************************************************************************
         \brief Constructor for the resource manager. Loads all the default assets for use.
        *************************************************************************************/
        ResourceManager();

        /*!***********************************************************************************
         \brief Destructor for the resource manager. Unloads all the default assets for use.
        *************************************************************************************/
        ~ResourceManager();

        std::shared_ptr<Graphics::Texture> m_defaultTexture;
        std::shared_ptr<AudioManager::Audio > m_defaultAudio;
        std::shared_ptr<Font> m_defaultFont;
        std::shared_ptr<Animation> m_defaultAnimation;

        std::string m_defaultTextureKey;
        std::string m_defaultAudioKey;
        std::string m_defaultFontKey;
        std::string m_defaultAnimationKey;
        std::string m_defaultPrefabKey;

        std::unordered_set<std::string> m_allTextureKeys;
        std::unordered_set<std::string> m_allAudioKeys;
        std::unordered_set<std::string> m_allFontKeys;
        std::unordered_set<std::string> m_allAnimationKeys;
        std::unordered_set<std::string> m_allPrefabKeys;
    };
}
