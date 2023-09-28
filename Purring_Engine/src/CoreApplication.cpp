/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     CoreApplication.cpp
 \creation date:       To check
 \last updated:        16-09-2023
 \author:              Brandon HO Jun Jie
 \co-author:           Hans (You Yang) ONG
 \co-author:           Jarran TAN Yan Zhi

 \par      email:      brandonjunjie.ho@digipen.edu
 \par      email:      youyang.o@digipen.edu
 \par      email:      jarranyanzhi.tan@digipen.edu

 \brief    This file contains the CoreApplication class, which serves as the entry point for
           the engine. It handles the main application loop, initializes and updates all registered 
           systems, and manages application-level resources such as the window and FPS controller.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/


/*                                                                                                          includes
--------------------------------------------------------------------------------------------------------------------- */
#include "prpch.h"

// imgui
#include "Editor/Editor.h"
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

// graphics
#include "Graphics/GLHeaders.h"

#include "CoreApplication.h"
#include "WindowManager.h"
#include "Logging/Logger.h"
#include "MemoryManager.h"

// Resource manager
#include "ResourceManager/ResourceManager.h"

// Audio Stuff - HANS
#include "AudioManager.h"

// Time
#include "Time/TimeManager.h"

// Physics and Collision
#include "Physics/RigidBody.h"
#include "Physics/Colliders.h"
#include "Physics/CollisionManager.h"
#include "Physics/PhysicsManager.h"

// Serialization
#include "Data/SerializationManager.h"

// ECS
#include "ECS//EntityFactory.h"
#include "ECS/Entity.h"
#include "ECS/Components.h"
#include "ECS/Prefabs.h"
#include "ECS/SceneView.h"

// Graphics
#include "Graphics/Renderer.h"
#include "InputSystem.h"

// testing
Logger engine_logger = Logger("ENGINE");
SerializationManager sm;

PE::EntityManager entManager;
PE::EntityFactory entFactory;


/*-----------------------------------------------------------------------------
/// <summary>
/// Constructor for the CoreApplication class.
/// Initializes variables and sets up the application window, FPS controller,
/// logging, and rendering system.
/// </summary>
----------------------------------------------------------------------------- */
PE::CoreApplication::CoreApplication()
{
    // Registers Components to ECS
    REGISTERCOMPONENT(RigidBody);
    REGISTERCOMPONENT(Collider);
    REGISTERCOMPONENT(Transform);
    REGISTERCOMPONENT(Graphics::Renderer);

	m_Running = true;
	m_lastFrameTime = 0;

    // Create and set up the window using WindowManager
    m_window = m_windowManager.InitWindow(1000, 1000, "Purring Engine");

    // Default to 60 FPS
    m_fpsController.SetTargetFPS(60);
    
    // set flags
    engine_logger.SetFlag(Logger::EnumLoggerFlags::WRITE_TO_CONSOLE | Logger::EnumLoggerFlags::WRITE_TO_FILE | Logger::EnumLoggerFlags::DEBUG, true);
    engine_logger.SetTime();
    engine_logger.AddLog(false, "Engine initialized!", __FUNCTION__);

    // Audio Stuff - HANS
    AudioManager::GetInstance()->Init();
    {
        engine_logger.AddLog(false, "Failed to initialize AudioManager", __FUNCTION__);
    }

    //create instance of memory manager (prob shld bring this out to entry point)
    MemoryManager::GetInstance();   

    // Add system to list & assigning memory to them
    Graphics::RendererManager* p_rendererManager = new (MemoryManager::GetInstance().AllocateMemory("Graphics Manager", sizeof(Graphics::RendererManager)))Graphics::RendererManager{m_window};
    PhysicsManager* p_physicsManager = new (MemoryManager::GetInstance().AllocateMemory("Physics Manager", sizeof(PhysicsManager)))PhysicsManager{};
    CollisionManager* p_collisionManager = new (MemoryManager::GetInstance().AllocateMemory("Collision Manager", sizeof(CollisionManager)))CollisionManager{};
    InputSystem* p_inputSystem = new (MemoryManager::GetInstance().AllocateMemory("Input System", sizeof(InputSystem)))InputSystem{};
    AddSystem(p_inputSystem);
    AddSystem(p_physicsManager);
    AddSystem(p_collisionManager);
    AddSystem(p_rendererManager);

    // Load a texture
    std::string catTextureName{ "cat" }, cat2TextureName{ "cat2" }, bgTextureName{ "bg" };
    ResourceManager::GetInstance().LoadTextureFromFile(catTextureName, "../Assets/Textures/Cat1_128x128.png");
    ResourceManager::GetInstance().LoadTextureFromFile(cat2TextureName, "../Assets/Textures/image2.png");
    ResourceManager::GetInstance().LoadTextureFromFile(bgTextureName, "../Assets/Textures/TempFrame.png");
    
    // Creates an entity that displays the background image
    EntityID id = g_entityFactory->CreateEntity();    
    g_entityFactory->Assign(id, { "Transform", "Renderer" });
    g_entityManager->Get<Transform>(id).position.x = 0.f;
    g_entityManager->Get<Transform>(id).position.y = 0.f;
    g_entityManager->Get<Transform>(id).width = 1000.f;
    g_entityManager->Get<Transform>(id).height = 1000.f;
    g_entityManager->Get<Transform>(id).orientation = 0.f;
    g_entityManager->Get<Graphics::Renderer>(id).SetTextureKey(bgTextureName);
    g_entityManager->Get<Graphics::Renderer>(id).SetColor(1.f, 1.f, 1.f, 1.f);
    
    // Creates an entity that is attached to the Character Controller
    EntityID id2 = g_entityFactory->CreateFromPrefab("GameObject");
    g_entityManager->Get<Transform>(id2).position.x = 0.f;
    g_entityManager->Get<Transform>(id2).position.y = 0.f;
    g_entityManager->Get<Transform>(id2).width = 100.f;
    g_entityManager->Get<Transform>(id2).height = 100.f;
    g_entityManager->Get<Transform>(id2).orientation = 0.f;
    g_entityManager->Get<RigidBody>(id2).SetType(EnumRigidBodyType::DYNAMIC);
    g_entityManager->Get<Collider>(id2).colliderVariant = CircleCollider();
    g_entityManager->Get<Graphics::Renderer>(id2).SetTextureKey(catTextureName);
    g_entityManager->Get<Graphics::Renderer>(id2).SetColor(1.f, 1.f, 1.f);
    g_entityManager->Get<RigidBody>(id2).SetMass(10.f);
    
}

/*-----------------------------------------------------------------------------
/// <summary>
/// Destructor for the CoreApplication class.
/// Responsible for cleaning up resources.
/// </summary>
----------------------------------------------------------------------------- */
PE::CoreApplication::~CoreApplication()
{
	// anything for destructor to do?
}



/*-----------------------------------------------------------------------------
/// <summary>
/// Main loop for the CoreApplication class.
/// Controls the game loop, updates systems, and handles user input and FPS.
/// </summary>
----------------------------------------------------------------------------- */
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

        // Iterate through the list of keys and check if any are pressed
        for (int key : keys)
        {
            // Update target FPS if a key is pressed
            if (glfwGetKey(m_window, key) == GLFW_PRESS)
            {
                m_fpsController.UpdateTargetFPSBasedOnKey(key);
            }
        }

        //Audio Stuff - HANS
        AudioManager::GetInstance()->Update();

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
        if (g_entityManager->GetComponentPool<Transform>().HasEntity(1))
            Graphics::RendererManager::m_mainCamera.SetPosition(g_entityManager->Get<Transform>(1).position.x, g_entityManager->Get<Transform>(1).position.y);

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




/*-----------------------------------------------------------------------------
/// <summary>
/// Initializes all registered systems in CoreApplication.
/// Iterates through each system in the system list and calls their
/// respective InitializeSystem function.
/// </summary>
----------------------------------------------------------------------------- */
void PE::CoreApplication::InitSystems()
{
    // Init all systems and iterate through each system in m_systemList and initialize it
    for (System* system : m_systemList)
    {
        system->InitializeSystem();                             // Call the InitializeSystem method for each system
    }
}



/*-----------------------------------------------------------------------------
/// <summary>
/// Destroys all registered systems in CoreApplication.
/// Iterates through each system in the system list, calls their respective
/// DestroySystem function, and then deletes them.
/// </summary>
----------------------------------------------------------------------------- */
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



/*-----------------------------------------------------------------------------
/// <summary>
/// Adds a system to the CoreApplication's system list.
/// Appends the given system pointer to the end of the system list,
/// </summary>
///
/// <param name="system">
/// A pointer to the system that will be managed by CoreApplication.
/// </param>
----------------------------------------------------------------------------- */
void PE::CoreApplication::AddSystem(System* system)
{
    // Add a system to CoreApplication append the provided system pointer to the m_systemList vector
    m_systemList.push_back(system);
}
