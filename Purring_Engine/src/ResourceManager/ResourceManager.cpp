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
#include "Logging/Logger.h"

extern Logger engine_logger;

#define REGISTERPREFAB(prefabName) PE::ResourceManager::GetInstance().AddPrefabKeyToLoad(prefabName)

namespace PE
{
    ResourceManager::ResourceManager()
                    : m_defaultTexture{ std::make_shared<Graphics::Texture>() },
                      m_defaultAudio{ std::make_shared<AudioManager::Audio>() },
                      m_defaultFont{ std::make_shared<Font>() },
                      m_defaultAnimation{ std::make_shared<Animation>() },
                      m_defaultCursorTexture{ std::make_shared<Graphics::Texture>() },
                      m_hoverCursorTexture{ std::make_shared<Graphics::Texture>() },
                      m_defaultTextureKey{ "../Assets/Defaults/Default_Texture_512px.png" },
                      m_defaultAudioKey{ "../Assets/Defaults/Default_Audio.wav" },
                      m_defaultFontKey{ "../Assets/Defaults/Default_Font.ttf" },
                      m_defaultAnimationKey{ "../Assets/Defaults/Default_Animation.anim" },
                      m_defaultPrefabKey{ "../Assets/Defaults/Default_Prefab.prefab" }
        {
#ifndef GAMERELEASE
            // load editor icons
            LoadIconFromFile("../Assets/Icons/Directory_Icon.png", "../Assets/Icons/Directory_Icon.png");
            LoadIconFromFile("../Assets/Icons/Audio_Icon.png", "../Assets/Icons/Audio_Icon.png");
            LoadIconFromFile("../Assets/Icons/Font_Icon.png", "../Assets/Icons/Font_Icon.png");
            LoadIconFromFile("../Assets/Icons/Prefabs_Icon.png", "../Assets/Icons/Prefabs_Icon.png");
            LoadIconFromFile("../Assets/Icons/Texture_Icon.png", "../Assets/Icons/Texture_Icon.png");
            LoadIconFromFile("../Assets/Icons/Other_Icon.png", "../Assets/Icons/Other_Icon.png");
#endif // !GAMERELEASE
        }

    ResourceManager::~ResourceManager()
    {
        Icons.clear();
        ShaderPrograms.clear();
        m_defaultTexture.reset();
        m_defaultAudio.reset();
        m_defaultFont.reset();
        m_defaultAnimation.reset();

        m_defaultCursorTexture.reset();
        m_hoverCursorTexture.reset();
        
#ifdef RESOURCE_LOG

        const char* filepath = "../Assets/Settings/resourcelog.txt";

        std::ofstream outfile(filepath);
        if (outfile.is_open())
        {
            for (auto const& str : m_allResourcesUsed)
            {
                outfile << str << std::endl;
            }

            outfile.close();
        }
        else
        {
            std::cerr << "Could not open the file for writing: " << filepath << std::endl;
        }
#endif // RESOURCE_LOG
    }

    void ResourceManager::LoadDefaultAssets()
    {
        // initialize default assets
        m_defaultTexture->CreateTexture(m_defaultTextureKey);
        m_defaultAudio->LoadSound(m_defaultAudioKey, AudioManager::GetInstance().GetFMODSystem());
        m_defaultFont->Initialize(m_defaultFontKey);
        m_defaultAnimation->LoadAnimation(m_defaultAnimationKey);


        // Load cursor textures from configuration file
        std::ifstream configFile("../Assets/Settings/config.json");
        if (configFile)
        {
            // Read info from the JSON file
            nlohmann::json configJson;
            configFile >> configJson;

            try
            {
                // Load texture from file
                m_defaultCursorTexture->CreateTexture(configJson["cursor"]["cursorDefaultTexture"]); //"../Assets/Textures/Cursor/CustomCursor_32px.png");
                m_hoverCursorTexture->CreateTexture(configJson["cursor"]["cursorHoverTexture"]); //"../Assets/Textures/Cursor/PointyCursor_32px.png");
            }
            catch (...)
            {
                std::cerr << "ResourceManager::LoadDefaultAssets(): Cursor texture keys could not be found in the JSON file" << std::endl;
            }

        }
    }

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

    bool ResourceManager::LoadTextureFromFile(std::string const& r_name, std::string const& r_filePath)
    {
        // check if texture is empty
        if (r_name == "")
        {
            return false;
        }
#ifdef RESOURCE_LOG
        m_allResourcesUsed.insert(r_name);
#endif

        // check if texture is already loaded
        if (Textures.find(r_name) == Textures.end())
        {
            Textures[r_name] = std::make_shared<Graphics::Texture>();


            if (!Textures[r_name]->CreateTexture(r_filePath))
            {
                // fail to create texture, delete key
                engine_logger.SetFlag(Logger::EnumLoggerFlags::WRITE_TO_CONSOLE | Logger::EnumLoggerFlags::DEBUG, true);
                engine_logger.SetTime();
                engine_logger.AddLog(false, "Texture " + r_name + " does not exist.", __FUNCTION__);

                Textures.erase(r_name);
                return false;
            }
        }

        return true;
    }

    bool ResourceManager::LoadIconFromFile(std::string const& r_name, std::string const& r_filePath)
    {
        // check if icon is empty
        if (r_name == "")
        {
            return false;
        }

        // check if icon is already loaded
        if (Icons.find(r_name) == Icons.end())
        {
            Icons[r_name] = std::make_shared<Graphics::Texture>();
            if (!Icons[r_name]->CreateTexture(r_filePath))
            {
                // fail to create texture, delete key
                engine_logger.SetFlag(Logger::EnumLoggerFlags::WRITE_TO_CONSOLE | Logger::EnumLoggerFlags::DEBUG, true);
                engine_logger.SetTime();
                engine_logger.AddLog(false, "Icon " + r_name + " does not exist.", __FUNCTION__);

                Icons.erase(r_name);
                return false;
            }
        }
        return true;
    }

    bool ResourceManager::LoadAudioFromFile(std::string const& r_key, std::string const& r_filePath)
    {
        // check if audio is empty
        if (r_key == "")
        {
            return false;
        }

#ifdef RESOURCE_LOG
        m_allResourcesUsed.insert(r_key);
#endif

        // check if audio is already loaded
        if (Sounds.find(r_key) == Sounds.end())
        {
            Sounds[r_key] = std::make_shared<AudioManager::Audio>();

            if (AudioManager::GetInstance().GetFMODSystem() == nullptr)
            {
                std::cout << "NO SYSTEM";
                return false;
            }

            if (!Sounds[r_key]->LoadSound(r_filePath, AudioManager::GetInstance().GetFMODSystem()))
            {
                std::cout << "Fail to load sound" << r_filePath << std::endl;
                // fail to load sound, delete key
                Sounds.erase(r_key);
                return false;
            }
        }
        return true;
    }

    std::string ResourceManager::LoadDraggedAudio(std::string const& r_filePath)
    {        
        // Extract the file name with the extension
        std::filesystem::path filePath(r_filePath);
        std::cout << "[LoadDraggedAudio] Loading audio from path: " << r_filePath << std::endl;

        std::string key = filePath.filename().string(); // Key generation with file extension
        std::cout << "[LoadDraggedAudio] Generated key: " << key << std::endl;

        // Load the audio file
        Sounds[key] = std::make_shared<AudioManager::Audio>();

        if (AudioManager::GetInstance().GetFMODSystem() == nullptr)
        {
            std::cout << "NO SYSTEM";
        }

        if (!Sounds[key]->LoadSound(r_filePath, AudioManager::GetInstance().GetFMODSystem()))
        {
            std::cout << "Failed to load sound from path: " << r_filePath << std::endl;
            Sounds.erase(key);
            return "";
        }

        std::cout << "Loaded sound: " << key << " from path: " << r_filePath << std::endl;
        return key;
    }

    bool ResourceManager::LoadFontFromFile(std::string const& r_key, std::string const& r_filePath)
    {
        // check if font is empty
        if (r_key == "")
        {
            return false;
        }

#ifdef RESOURCE_LOG
        m_allResourcesUsed.insert(r_key);
#endif

        // check if font is already loaded
        if (Fonts.find(r_key) == Fonts.end())
        {
            Fonts[r_key] = std::make_shared<Font>();

            if (!Fonts[r_key]->Initialize(r_filePath))
            {
                // fail to load font, delete key
                engine_logger.SetFlag(Logger::EnumLoggerFlags::WRITE_TO_CONSOLE | Logger::EnumLoggerFlags::DEBUG, true);
                engine_logger.SetTime();
                engine_logger.AddLog(false, "Font " + r_key + " does not exist.", __FUNCTION__);

                Fonts.erase(r_key);
                return false;
            }
        }

        return true;
    }

    bool ResourceManager::LoadAnimationFromFile(std::string const& r_key, std::string const& r_filePath)
    {
        // check if animation is empty
        if (r_key == "")
        {
            return false;
        }

#ifdef RESOURCE_LOG
        m_allResourcesUsed.insert(r_key);
#endif

        // check if animation is already loaded
        if (Animations.find(r_key) == Animations.end())
        {
            Animations[r_key] = std::make_shared<Animation>();
            Animations[r_key]->SetAnimationID(r_key);

            if (!Animations[r_key]->LoadAnimation(r_filePath))
            {
                // fail to load animation, delete key
                engine_logger.SetFlag(Logger::EnumLoggerFlags::WRITE_TO_CONSOLE | Logger::EnumLoggerFlags::DEBUG, true);
                engine_logger.SetTime();
                //engine_logger.AddLog(false, "Animation " + r_key + " does not exist.", __FUNCTION__);

                Animations.erase(r_key);
                return false;
            }
        }

        return true;
    }

    size_t ResourceManager::LoadPrefabFromFile(std::string const& r_filePath, bool useFilePath)
    {
        std::filesystem::path filepath;
        SerializationManager serializationManager;

        // if using filepath
        if (useFilePath)
        {
            filepath = r_filePath;
        }
        else
        {
            filepath = std::string{ "../Assets/Prefabs/" } + r_filePath;
        }
        if (!std::filesystem::exists(filepath))
        {
            std::cerr << "File does not exist: " << filepath << std::endl;
            return serializationManager.CreateEntityFromPrefab(m_defaultPrefabKey);
        }     

#ifdef RESOURCE_LOG
        m_allResourcesUsed.insert(filepath.string());
#endif

        size_t id = serializationManager.CreateEntityFromPrefab(filepath.string());

        // if prefab not found
        if (id == MAXSIZE_T)
        {
            serializationManager.CreateEntityFromPrefab(m_defaultPrefabKey);
        }

        return id;
    }

    //size_t ResourceManager::GetPrefab(std::string const& r_name)
    //{
    //    SerializationManager serializationManager;
    //    // if prefab is not found
    //    if (Prefabs.find(r_name) == Prefabs.end())
    //    {
    //        engine_logger.SetFlag(Logger::EnumLoggerFlags::WRITE_TO_CONSOLE | Logger::EnumLoggerFlags::DEBUG, true);
    //        engine_logger.SetTime();
    //        engine_logger.AddLog(false, "Prefab " + r_name + " not loaded, loading prefab.", __FUNCTION__);

    //        // load texture
    //        if (LoadTextureFromFile(r_name, r_name))
    //        {
    //            return serializationManager.CreateEntityFromPrefab(Prefabs[r_name]);
    //        }
    //        else
    //        {
    //            // return default prefab change
    //            return Prefabs[r_name];
    //        }
    //    }

    //    // if found, create entity from prefab
    //    return serializationManager.CreateEntityFromPrefab(Prefabs[r_name]);
    //}

    ImVec2 ResourceManager::GetTextureSize(const std::string& r_name)
    {
        auto it = Icons.find(r_name);
        if (it != Icons.end()) {
            auto texture = it->second;
            return ImVec2(static_cast<float>(texture->GetWidth()), static_cast<float>(texture->GetHeight()));
        }
        else
        {
            // If Texture not found, maybe load a PURPLE icon? @Brandon -Hans
            return ImVec2(1.0f, 1.0f); // Default size to avoid division by zero in aspect ratio calculations
        }
    }

    std::string ResourceManager::LoadTexture(std::string const& r_fileName)
    {
        std::string filePath = "../Assets/Textures/Resources/" + r_fileName;

        // if texture is loaded, return key
        if(LoadTextureFromFile(filePath, filePath))
        {
            return filePath;
        }

        return m_defaultTextureKey;
    }

    std::string ResourceManager::LoadAudio(std::string const& r_fileName)
    {
        std::string filePath = "../Assets/Audio/Resources/" + r_fileName;

        // if audio is loaded, return key
        if (LoadAudioFromFile(filePath, filePath))
        {
            return filePath;
        }

        return m_defaultAudioKey;
    }

    std::string ResourceManager::LoadFont(std::string const& r_fileName)
    {
        std::string filePath = "../Assets/Fonts/Resources/" + r_fileName;

        // if texture is loaded, return key
        if (LoadFontFromFile(filePath, filePath))
        {
            return filePath;
        }

        return m_defaultFontKey;
    }

    std::string ResourceManager::LoadAnimation(std::string const& r_fileName)
    {
        std::string filePath = "../Assets/Animation/Resources/" + r_fileName;

        // if texture is loaded, return key
        if (LoadAnimationFromFile(filePath, filePath))
        {
            return filePath;
        }

        return m_defaultAnimationKey;
    }

    std::shared_ptr<Graphics::Texture> ResourceManager::GetTexture(std::string const& r_name)
    {
        // if texture is not found
        if (Textures.find(r_name) == Textures.end())
        {
            engine_logger.SetFlag(Logger::EnumLoggerFlags::WRITE_TO_CONSOLE | Logger::EnumLoggerFlags::DEBUG, true);
            engine_logger.SetTime();
            engine_logger.AddLog(false, "Texture " + r_name + " not loaded, loading texture.", __FUNCTION__);

            // load texture
            if (LoadTextureFromFile(r_name, r_name))
            {
                return Textures[r_name];
            }
            else
            {
                return m_defaultTexture;
            }
        }

        return Textures[r_name];
    }

    std::shared_ptr<Graphics::Texture> ResourceManager::GetIcon(std::string const& r_name)
    {
        // if texture is not found
        if (Icons.find(r_name) == Icons.end())
        {
            engine_logger.SetFlag(Logger::EnumLoggerFlags::WRITE_TO_CONSOLE | Logger::EnumLoggerFlags::DEBUG, true);
            engine_logger.SetTime();
            engine_logger.AddLog(false, "Icon " + r_name + " not loaded, loading icon.", __FUNCTION__);

            // load texture
            if (LoadIconFromFile(r_name, r_name))
            {
                return Icons[r_name];
            }
            else
            {
                return m_defaultTexture;
            }
        }

        return Icons[r_name];
    }

    std::shared_ptr<Animation> ResourceManager::GetAnimation(std::string const& r_name)
    {
        // if animation is not found, load it
        if (Animations.find(r_name) == Animations.end())
        {
            engine_logger.SetFlag(Logger::EnumLoggerFlags::WRITE_TO_CONSOLE | Logger::EnumLoggerFlags::DEBUG, true);
            engine_logger.SetTime();
            //engine_logger.AddLog(false, "Animation " + r_name + " not loaded, loading animation.", __FUNCTION__);

            // load animation
            if (LoadAnimationFromFile(r_name, r_name))
            {
                return Animations[r_name];
            }
            else
            {
                // return default animation
                return m_defaultAnimation;
            }
		}

        return Animations[r_name];
	}

    std::shared_ptr<Font> ResourceManager::GetFont(std::string const& r_name)
    {
        // if font is not found, load it
        if (Fonts.find(r_name) == Fonts.end())
        {
            engine_logger.SetFlag(Logger::EnumLoggerFlags::WRITE_TO_CONSOLE | Logger::EnumLoggerFlags::DEBUG, true);
            engine_logger.SetTime();
            engine_logger.AddLog(false, "Font " + r_name + " not loaded, loading font.", __FUNCTION__);

            // load font
            if (LoadFontFromFile(r_name, r_name))
            {
                return Fonts[r_name];
            }
            else
            {
                // return default font
                return m_defaultFont;
            }
        }

        return Fonts[r_name];
    }

    std::shared_ptr<AudioManager::Audio> ResourceManager::GetAudio(std::string const& r_name)
    {
        // if audio is not found, load it
        if (Sounds.find(r_name) == Sounds.end())
        {
            engine_logger.SetFlag(Logger::EnumLoggerFlags::WRITE_TO_CONSOLE | Logger::EnumLoggerFlags::DEBUG, true);
            engine_logger.SetTime();
            engine_logger.AddLog(false, "Audio " + r_name + " not loaded, loading audio.", __FUNCTION__);

            // load audio
            if (LoadAudioFromFile(r_name, r_name))
            {
                return Sounds[r_name];
            }
            else
            {
                // return default audio
                return nullptr;
            }
        }

        return Sounds[r_name];
    }

    void ResourceManager::LoadAllTextures()
    {
        engine_logger.SetFlag(Logger::EnumLoggerFlags::WRITE_TO_CONSOLE | Logger::EnumLoggerFlags::DEBUG, true);
        engine_logger.SetTime();
        engine_logger.AddLog(false, "Loading all textures", __FUNCTION__);
        for (auto const& itr : m_allTextureKeys)
        {
            engine_logger.AddLog(false, itr, __FUNCTION__);
            LoadTextureFromFile(itr, itr);
        }
    }

    void ResourceManager::LoadAllAudio()
    {
        engine_logger.SetFlag(Logger::EnumLoggerFlags::WRITE_TO_CONSOLE | Logger::EnumLoggerFlags::DEBUG, true);
        engine_logger.SetTime();
        engine_logger.AddLog(false, "Loading all audio", __FUNCTION__);
        for (auto const& itr : m_allAudioKeys)
        {
            engine_logger.AddLog(false, itr, __FUNCTION__);
            LoadAudioFromFile(itr, itr);
        }
    }

    void ResourceManager::LoadAllFonts()
    {
        engine_logger.SetFlag(Logger::EnumLoggerFlags::WRITE_TO_CONSOLE | Logger::EnumLoggerFlags::DEBUG, true);
        engine_logger.SetTime();
        engine_logger.AddLog(false, "Loading all fonts", __FUNCTION__);
        for (auto const& itr : m_allFontKeys)
        {
            engine_logger.AddLog(false, itr, __FUNCTION__);
			LoadFontFromFile(itr, itr);
		}
    }

    void ResourceManager::LoadAllAnimations()
    {
        engine_logger.SetFlag(Logger::EnumLoggerFlags::WRITE_TO_CONSOLE | Logger::EnumLoggerFlags::DEBUG, true);
        engine_logger.SetTime();
        engine_logger.AddLog(false, "Loading all animations", __FUNCTION__);
        for (auto const& itr : m_allAnimationKeys)
        {
            engine_logger.AddLog(false, itr, __FUNCTION__);
			LoadAnimationFromFile(itr, itr);
		}
    }

    void ResourceManager::LoadAllPrefabs()
    {
        //engine_logger.SetFlag(Logger::EnumLoggerFlags::WRITE_TO_CONSOLE | Logger::EnumLoggerFlags::DEBUG, true);
        //engine_logger.SetTime();
        //engine_logger.AddLog(false, "Loading all animations", __FUNCTION__);
        //for (auto const& itr : m_allAnimationKeys)
        //{
        //    engine_logger.AddLog(false, itr, __FUNCTION__);
        //    LoadAnimationFromFile(itr, itr);
        //}
    }

    void ResourceManager::LoadAllResources()
    {
        LoadAllPrefabs();
        LoadAllAnimations();
        LoadAllTextures();
        LoadAllAudio();
        LoadAllFonts();
    }

    void ResourceManager::AddTextureKeyToLoad(std::string const& r_key)
    {
        m_allTextureKeys.insert(r_key);
    }

    void ResourceManager::AddAudioKeyToLoad(std::string const& r_key)
    {
        m_allAudioKeys.insert(r_key);
    }

    void ResourceManager::AddFontKeyToLoad(std::string const& r_key)
    {
        m_allFontKeys.insert(r_key);
    }

    void ResourceManager::AddAnimationKeyToLoad(std::string const& r_key)
    {
        m_allAnimationKeys.insert(r_key);
    }

    void ResourceManager::AddPrefabKeyToLoad(std::string const& r_key)
    {
        m_allPrefabKeys.insert(r_key);
    }

    std::shared_ptr<Graphics::Texture> ResourceManager::GetDefaultCursorTexture() const
    {
        if (!m_defaultCursorTexture)
            return m_defaultTexture;

        return m_defaultCursorTexture;
    }

    std::shared_ptr<Graphics::Texture> ResourceManager::GetHoverCursorTexture() const
    {
        if (!m_hoverCursorTexture)
            return m_defaultTexture;

        return m_hoverCursorTexture;
    }

    void ResourceManager::UnloadResources()
    {
        engine_logger.SetFlag(Logger::EnumLoggerFlags::WRITE_TO_CONSOLE | Logger::EnumLoggerFlags::DEBUG, true);
        engine_logger.SetTime();
        engine_logger.AddLog(false, "Unloading all resources", __FUNCTION__);

        Textures.clear();
        Sounds.clear();
        Fonts.clear();
        Animations.clear();

        m_allTextureKeys.clear();
        m_allAudioKeys.clear();
        m_allFontKeys.clear();
        m_allAnimationKeys.clear();

        engine_logger.AddLog(false, "Resource unloaded successfully", __FUNCTION__);
    }
}
