/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     CoreApplication.cpp
 \date     28-08-2023

 \author               Brandon HO Jun Jie
 \par      email:      brandonjunjie.ho@digipen.edu

 \brief    This file contains the CoreApplication class, which serves as the entry point for
           the engine. It handles the main application loop, initializes and updates all registered 
           systems, and manages application-level resources such as the window and FPS controller.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/

/*                                                                                                          includes
--------------------------------------------------------------------------------------------------------------------- */
#include "prpch.h"

// ImGui Headers
#include "Editor/Editor.h"
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

// Graphics Headers
#include "Graphics/GLHeaders.h"
#include "Graphics/Renderer.h"

// Core Functionality
#include "CoreApplication.h"
#include "WindowManager.h"

// Logging and Memory
#include "Logging/Logger.h"
#include "Memory/MemoryManager.h"

// Resource Management
#include "ResourceManager/ResourceManager.h"

// Audio
#include "AudioManager/AudioManager.h"

// Time Management
#include "Time/TimeManager.h"

// Physics and Collision
#include "Physics/RigidBody.h"
#include "Physics/Colliders.h"
#include "Physics/CollisionManager.h"
#include "Physics/PhysicsManager.h"

// Serialization
#include "Data/SerializationManager.h"

// Entity Component System (ECS)
#include "ECS/EntityFactory.h"
#include "ECS/Entity.h"
#include "ECS/Components.h"
#include "ECS/Prefabs.h"
#include "ECS/SceneView.h"

// Input
#include "Input/InputSystem.h"

// Testing
Logger engine_logger = Logger("ENGINE");

PE::CoreApplication::CoreApplication()
{
    InitializeVariables();
    RegisterComponents();

    // Load Configuration
    std::ifstream configFile("config.json");
    nlohmann::json configJson;
    configFile >> configJson;
    int width = configJson["window"]["width"];
    int height = configJson["window"]["height"];
    // Initialize Window
    m_window = m_windowManager.InitWindow(width, height, "Purring_Engine");
    m_fpsController.SetTargetFPS(60);
    
    InitializeLogger();
    InitializeAudio();
    InitializeMemoryManager();
    InitializeSystems();


    // Load Textures and Animations
    std::string catTextureName{ "cat" }, cat2TextureName{ "cat2" }, bgTextureName{ "bg" };
    ResourceManager::GetInstance().LoadTextureFromFile(catTextureName, "../Assets/Textures/Cat_Grey_128px.png");
    ResourceManager::GetInstance().LoadTextureFromFile(cat2TextureName, "../Assets/Textures/Cat_Grey_Blink_128px.png");
    ResourceManager::GetInstance().LoadTextureFromFile(bgTextureName, "../Assets/Textures/TempFrame.png");

    // Animation textures
    // Animation 1
    ResourceManager::GetInstance().LoadTextureFromFile("catAnim1", "../Assets/Textures/CatSprite/Cat_Grey_128px1.png");
    ResourceManager::GetInstance().LoadTextureFromFile("catAnim2", "../Assets/Textures/CatSprite/Cat_Grey_128px2.png");
    ResourceManager::GetInstance().LoadTextureFromFile("catAnim3", "../Assets/Textures/CatSprite/Cat_Grey_128px3.png");
    ResourceManager::GetInstance().LoadTextureFromFile("catAnim4", "../Assets/Textures/CatSprite/Cat_Grey_128px4.png");
    ResourceManager::GetInstance().LoadTextureFromFile("catAnim5", "../Assets/Textures/CatSprite/Cat_Grey_128px5.png");

    // Animation 2
    ResourceManager::GetInstance().LoadTextureFromFile("cat2Anim1", "../Assets/Textures/CatSprite2/Cat_Grey_128px_Walk_2.png");
    ResourceManager::GetInstance().LoadTextureFromFile("cat2Anim2", "../Assets/Textures/CatSprite2/Cat_Grey_128px_Walk_3.png");

    SerializationManager serializationManager;
    //create background from file
    serializationManager.LoadFromFile("../Assets/Prefabs/Background_Prefab.json");
    
    // Creates an entity from file that is attached to the Character Controller
    serializationManager.LoadFromFile("../Assets/Prefabs/Player_Prefab.json");
    

}

PE::CoreApplication::~CoreApplication()
{
    // future stuff can add here
}

void PE::CoreApplication::Run()
{
    // Start engine run time
    TimeManager::GetInstance().EngineStart();

    // Main Application Loop
    // Continue until the GLFW window is flagged to close
    while (!glfwWindowShouldClose(m_window))
    {
        // Time start
        TimeManager::GetInstance().StartFrame();
        engine_logger.SetTime();
        MemoryManager::GetInstance().CheckMemoryOver();
        
        // ----- UPDATE ----- //
        
        // List of keys to check for FPS adjustment
        const int keys[] = { GLFW_KEY_1, GLFW_KEY_2, GLFW_KEY_3, GLFW_KEY_4, GLFW_KEY_5, GLFW_KEY_6, GLFW_KEY_7, GLFW_KEY_8 };

        //// Iterate through the list of keys and check if any are pressed
        for (int key : keys)
        {
            // Update target FPS if a key is pressed
            if (glfwGetKey(m_window, key) == GLFW_PRESS)
            {
                m_fpsController.UpdateTargetFPSBasedOnKey(key);
            }
        }
        if (glfwGetKey(m_window, GLFW_KEY_L) == GLFW_PRESS)
        {
            try
            {
                std::vector testVector = { 1 };
                testVector[0] = testVector.at(1);
            }
            catch (const std::out_of_range& r_err)
            {
                engine_logger.AddLog(true, r_err.what(), __FUNCTION__);
                throw r_err;
            }
        }

        //Audio Stuff - HANS
        AudioManager::GetInstance().Update();

        // engine_logger.AddLog(false, "Frame rendered", __FUNCTION__);
        // Update the window title to display FPS (every second)
        double currentTime = glfwGetTime();
        if (currentTime - m_lastFrameTime >= 1.0)
        {
            m_windowManager.UpdateTitle(m_window, m_fpsController.GetFPS());
            m_lastFrameTime = currentTime;
        }

        // Iterate over and update all systems
        for (unsigned int i{ 0 }; i < m_systemList.size(); ++i)
        {
            TimeManager::GetInstance().SystemStartFrame();
            m_systemList[i]->UpdateSystem(TimeManager::GetInstance().GetDeltaTime()); //@TODO: Update delta time value here!!!!!!!!!!!!!!!!!!!!!!!!!!!
            TimeManager::GetInstance().SystemEndFrame(i);
        }
        //if (EntityManager::GetInstance().GetComponentPool<Transform>().HasEntity(1))
        //    Graphics::RendererManager::m_mainCamera.SetPosition(EntityManager::GetInstance().Get<Transform>(1).position.x, EntityManager::GetInstance().Get<Transform>(1).position.y);

        // Flush log entries
        engine_logger.FlushLog();

        TimeManager::GetInstance().EndFrame();
        // Finalize FPS calculations for the current frame
        m_fpsController.EndFrame();
    }

    // Cleanup for ImGui
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    // Additional Cleanup (if required)
    m_windowManager.Cleanup();
    ResourceManager::GetInstance().UnloadResources();
}

void PE::CoreApplication::Initialize()
{
    // Init all systems and iterate through each system in m_systemList and initialize it
    for (System* system : m_systemList)
    {
        system->InitializeSystem();                             // Call the InitializeSystem method for each system
    }
}

void PE::CoreApplication::DestroySystems()
{
    //memory auto deallocated by memory manager

     //destroy all systems
    for (System* system : m_systemList)
    {
        system->DestroySystem();
        system->~System();
        //delete system;
    }
}

void PE::CoreApplication::AddSystem(System* system)
{
    // Add a system to CoreApplication append the provided system pointer to the m_systemList vector
    m_systemList.push_back(system);
}


void PE::CoreApplication::InitializeVariables()
{
    m_Running = true;
    m_lastFrameTime = 0;
}

void PE::CoreApplication::RegisterComponents()
{
    REGISTERCOMPONENT(RigidBody);
    REGISTERCOMPONENT(Collider);
    REGISTERCOMPONENT(Transform);
    REGISTERCOMPONENT(Graphics::Renderer);
}

void PE::CoreApplication::InitializeLogger()
{
    engine_logger.SetFlag(Logger::EnumLoggerFlags::WRITE_TO_CONSOLE | Logger::EnumLoggerFlags::WRITE_TO_FILE | Logger::EnumLoggerFlags::DEBUG, true);
    engine_logger.SetTime();
    engine_logger.AddLog(false, "Engine initialized!", __FUNCTION__);
}


void PE::CoreApplication::InitializeAudio()
{
    AudioManager::GetInstance().Init();
    {
        engine_logger.AddLog(false, "AudioManager initialized!", __FUNCTION__);
    }
    ResourceManager::GetInstance().LoadAudioFromFile("audio_sound1", "../Assets/Audio/audioSFX_sound1.mp3");
    ResourceManager::GetInstance().LoadAudioFromFile("audio_sound2", "../Assets/Audio/audioSFX_sound2.mp3");
}


void PE::CoreApplication::InitializeMemoryManager()
{
    MemoryManager::GetInstance();
}

void PE::CoreApplication::InitializeSystems()
{
    // Add system to list & assigning memory to them
    Graphics::RendererManager* p_rendererManager = new (MemoryManager::GetInstance().AllocateMemory("Graphics Manager", sizeof(Graphics::RendererManager)))Graphics::RendererManager{ m_window };
    PhysicsManager* p_physicsManager = new (MemoryManager::GetInstance().AllocateMemory("Physics Manager", sizeof(PhysicsManager)))PhysicsManager{};
    CollisionManager* p_collisionManager = new (MemoryManager::GetInstance().AllocateMemory("Collision Manager", sizeof(CollisionManager)))CollisionManager{};
    InputSystem* p_inputSystem = new (MemoryManager::GetInstance().AllocateMemory("Input System", sizeof(InputSystem)))InputSystem{};
    AddSystem(p_inputSystem);
    AddSystem(p_physicsManager);
    AddSystem(p_collisionManager);
    AddSystem(p_rendererManager);
}