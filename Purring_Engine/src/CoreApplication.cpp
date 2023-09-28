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

    int width{ 1000 }, height{ 1000 };
    glfwGetWindowSize(m_window, &width, &height);
    EntityID id = EntityFactory::GetInstance().CreateEntity();
    EntityFactory::GetInstance().Assign(id, { "Transform", "Renderer" });
    EntityManager::GetInstance().Get<Transform>(id).position.x = 0.f;
    EntityManager::GetInstance().Get<Transform>(id).position.y = 0.f;
    EntityManager::GetInstance().Get<Transform>(id).width = static_cast<float>(width);
    EntityManager::GetInstance().Get<Transform>(id).height = static_cast<float>(height);
    EntityManager::GetInstance().Get<Transform>(id).orientation = 0.f;
    EntityManager::GetInstance().Get<Graphics::Renderer>(id).SetTextureKey(bgTextureName);
    EntityManager::GetInstance().Get<Graphics::Renderer>(id).SetColor(1.f, 1.f, 1.f, 1.f);

    EntityFactory::GetInstance().CreateFromPrefab("GameObject");
    EntityFactory::GetInstance().CreateFromPrefab("GameObject");

    // ----- Limit Test for Physics ----- //
    //std::random_device rd;
    //std::mt19937 gen(rd());
    //for (size_t i{ 2 }; i < 20; ++i)
    //{
    //    EntityID id = EntityFactory::GetInstance().CreateFromPrefab("GameObject");
    //
    //    std::uniform_int_distribution<>distr0(-550, 550);
    //    EntityManager::GetInstance().Get<Transform>(id).position.x = static_cast<float>(distr0(gen));
    //    std::uniform_int_distribution<>distr1(-250, 250);
    //    EntityManager::GetInstance().Get<Transform>(id).position.y = static_cast<float>(distr1(gen));
    //    std::uniform_int_distribution<>distr2(10, 200);
    //    EntityManager::GetInstance().Get<Transform>(id).width = static_cast<float>(distr2(gen));
    //    EntityManager::GetInstance().Get<Transform>(id).height = static_cast<float>(distr2(gen));
    //    EntityManager::GetInstance().Get<Transform>(id).orientation = 0.f;
    //
    //    if (i%3)
    //        EntityManager::GetInstance().Get<RigidBody>(id).SetType(EnumRigidBodyType::DYNAMIC);
    //    
    //    if (i%2)
    //        EntityManager::GetInstance().Get<Collider>(id).colliderVariant = CircleCollider();
    //    else
    //        EntityManager::GetInstance().Get<Collider>(id).colliderVariant = AABBCollider();
    //}

    // Make the first gameobject with a collider circle at world pos (100, 100)
    EntityManager::GetInstance().Get<Transform>(1).position.x = 0.f;
    EntityManager::GetInstance().Get<Transform>(1).position.y = 0.f;
    EntityManager::GetInstance().Get<Transform>(1).width = 100.f;
    EntityManager::GetInstance().Get<Transform>(1).height = 100.f;
    EntityManager::GetInstance().Get<Transform>(1).orientation = 0.f;
    EntityManager::GetInstance().Get<RigidBody>(1).SetType(EnumRigidBodyType::DYNAMIC);
    EntityManager::GetInstance().Get<Collider>(1).colliderVariant = CircleCollider();
    EntityManager::GetInstance().Get<Graphics::Renderer>(1).SetTextureKey(catTextureName);
    EntityManager::GetInstance().Get<Graphics::Renderer>(1).SetColor(1.f, 1.f, 0.f);
    EntityManager::GetInstance().Get<RigidBody>(1).SetMass(10.f);

    // Make the second gameobject a rectangle with an AABB collider at world pos (-100, -100)
    EntityManager::GetInstance().Get<Transform>(2).position.x = -100.f;
    EntityManager::GetInstance().Get<Transform>(2).position.y = -100.f;
    EntityManager::GetInstance().Get<Transform>(2).width = 50.f;
    EntityManager::GetInstance().Get<Transform>(2).height = 200.f;
    EntityManager::GetInstance().Get<Transform>(2).orientation = 0.f;
    EntityManager::GetInstance().Get<RigidBody>(2).SetType(EnumRigidBodyType::STATIC);
    EntityManager::GetInstance().Get<Collider>(2).colliderVariant = AABBCollider();
    EntityManager::GetInstance().Get<Collider>(2).isTrigger = true;

    // Render grid of 500 cat 2
    for (size_t i{}; i < 500; ++i) {
        EntityID id2 = EntityFactory::GetInstance().CreateEntity();
        EntityFactory::GetInstance().Assign(id2, { "Transform", "Renderer" });
        EntityManager::GetInstance().Get<Transform>(id2).position.x = 20.f * (i % 40) - 300.f;
        EntityManager::GetInstance().Get<Transform>(id2).position.y = 20.f * (i / 40) - 300.f;
        EntityManager::GetInstance().Get<Transform>(id2).width = 20.f;
        EntityManager::GetInstance().Get<Transform>(id2).height = 20.f;
        EntityManager::GetInstance().Get<Transform>(id2).orientation = 0.f;
        EntityManager::GetInstance().Get<Graphics::Renderer>(id2).SetTextureKey(cat2TextureName);
        EntityManager::GetInstance().Get<Graphics::Renderer>(id2).SetColor(1.f, 0.f, 1.f, 0.1f);
    }

    // Render grid of 2000 cat 1
    for (size_t i{}; i < 2000; ++i) {
        EntityID id2 = EntityFactory::GetInstance().CreateEntity();
        EntityFactory::GetInstance().Assign(id2, { "Transform", "Renderer" });
        EntityManager::GetInstance().Get<Transform>(id2).position.x = 10.f * (i % 45) - 100.f;
        EntityManager::GetInstance().Get<Transform>(id2).position.y = 10.f * (i / 45) - 100.f;
        EntityManager::GetInstance().Get<Transform>(id2).width = 10.f;
        EntityManager::GetInstance().Get<Transform>(id2).height = 10.f;
        EntityManager::GetInstance().Get<Transform>(id2).orientation = 0.f;
        EntityManager::GetInstance().Get<Graphics::Renderer>(id2).SetTextureKey(catTextureName);
        EntityManager::GetInstance().Get<Graphics::Renderer>(id2).SetColor(0.f, 1.f, 1.f, 0.5f);
    }

    // Render grid of 100 red squares
    for (size_t i{}; i < 100; ++i) {
        EntityID id2 = EntityFactory::GetInstance().CreateEntity();
        EntityFactory::GetInstance().Assign(id2, { "Transform", "Renderer" });
        EntityManager::GetInstance().Get<Transform>(id2).position.x = 10.f * (i % 20) - 300.f;
        EntityManager::GetInstance().Get<Transform>(id2).position.y = 10.f * (i / 20);
        EntityManager::GetInstance().Get<Transform>(id2).width = 10.f;
        EntityManager::GetInstance().Get<Transform>(id2).height = 10.f;
        EntityManager::GetInstance().Get<Transform>(id2).orientation = 0.f;
        EntityManager::GetInstance().Get<Graphics::Renderer>(id2).SetColor(1.f, 0.f, 0.f, 0.5f);
    }
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

        Graphics::RendererManager::m_mainCamera.SetPosition(EntityManager::GetInstance().Get<Transform>(1).position.x, EntityManager::GetInstance().Get<Transform>(1).position.y);

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
