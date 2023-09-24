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

#include "Data/SerializationManager.h"
// testing
Logger engine_logger = Logger("ENGINE");
SerializationManager sm;
#include "Physics/RigidBody.h"
#include "Physics/Colliders.h"
#include "Physics/CollisionManager.h"
#include "Physics/PhysicsManager.h"
#include "ECS//EntityFactory.h"
#include "ECS/Entity.h"
#include "ECS/Components.h"
#include "ECS/Prefabs.h"
#include "ECS/SceneView.h"
#include "Graphics/Renderer.h"

#include <random>

PE::EntityManager entManager;
PE::EntityFactory entFactory;

std::queue<EntityID> lastEnt{};

/*-----------------------------------------------------------------------------
/// <summary>
/// Constructor for the CoreApplication class.
/// Initializes variables and sets up the application window, FPS controller,
/// logging, and rendering system.
/// </summary>
----------------------------------------------------------------------------- */
PE::CoreApplication::CoreApplication()
{
    REGISTERCOMPONENT(RigidBody);
    REGISTERCOMPONENT(Collider);
    REGISTERCOMPONENT(Transform);
    REGISTERCOMPONENT(Graphics::Renderer);


	m_Running = true;
	m_lastFrameTime = 0;

    // Create and set up the window using WindowManager
    m_window = m_windowManager.InitWindow(1000, 1000, "Purring_Engine");

    m_fpsController.SetTargetFPS(60);                   // Default to 60 FPS
    // set flags
    engine_logger.SetFlag(Logger::EnumLoggerFlags::WRITE_TO_CONSOLE | Logger::EnumLoggerFlags::DEBUG, true);
    engine_logger.SetTime();
    engine_logger.AddLog(false, "Engine initialized!", __FUNCTION__);


    // Audio Stuff - HANS
    
    AudioManager::GetInstance()->Init();
    {
        engine_logger.AddLog(false, "Failed to initialize AudioManager", __FUNCTION__);
    }
    //create instance of memory manager (prob shld bring this out to entry point)
    MemoryManager::GetInstance();
    //assignning memory manually to renderer manager
    Graphics::RendererManager* rendererManager = new (MemoryManager::GetInstance()->AllocateMemory("Graphics Manager", sizeof(Graphics::RendererManager)))Graphics::RendererManager{m_window};
    AddSystem(rendererManager);


    // Load a texture
    std::string catTextureName{ "cat" };
    ResourceManager::GetInstance()->LoadTextureFromFile(catTextureName, "../Assets/Textures/Cat1_128x128.png");

    for (size_t i{ 0 }; i < 9; ++i)
    {
        g_entityFactory->CreateFromPrefab("GameObject");
    }

    // Make the first gameobject with a collider circle at world pos (100, 100)
    g_entityManager->Get<Transform>(0).position.x = 0.f;
    g_entityManager->Get<Transform>(0).position.y = 0.f;
    g_entityManager->Get<Transform>(0).width = 100.f;
    g_entityManager->Get<Transform>(0).height = 100.f;
    g_entityManager->Get<Transform>(0).orientation = 0.f;
    g_entityManager->Get<RigidBody>(0).SetType(EnumRigidBodyType::DYNAMIC);
    g_entityManager->Get<Collider>(0).colliderVariant = AABBCollider();
    g_entityManager->Get<Graphics::Renderer>(0).SetTextureKey(catTextureName);
    g_entityManager->Get<Graphics::Renderer>(0).SetColor(1.f, 1.f, 0.f);

    // creates AABB map boundaries
    g_entityManager->Get<Transform>(1).position.x = 700.f;
    g_entityManager->Get<Transform>(1).position.y = 0.f;
    g_entityManager->Get<Transform>(1).width = 100.f;
    g_entityManager->Get<Transform>(1).height = 700.f;
    g_entityManager->Get<Transform>(1).orientation = 0.f;
    g_entityManager->Get<RigidBody>(1).SetType(EnumRigidBodyType::STATIC);
    g_entityManager->Get<Collider>(1).colliderVariant = AABBCollider();
    
    g_entityManager->Get<Transform>(2).position.x = -700.f;
    g_entityManager->Get<Transform>(2).position.y = 0.f;
    g_entityManager->Get<Transform>(2).width = 100.f;
    g_entityManager->Get<Transform>(2).height = 700.f;
    g_entityManager->Get<Transform>(2).orientation = 0.f;
    g_entityManager->Get<RigidBody>(2).SetType(EnumRigidBodyType::STATIC);
    g_entityManager->Get<Collider>(2).colliderVariant = AABBCollider();
    
    g_entityManager->Get<Transform>(3).position.x = 0.f;
    g_entityManager->Get<Transform>(3).position.y = 350.f;
    g_entityManager->Get<Transform>(3).width = 1500.f;
    g_entityManager->Get<Transform>(3).height = 100.f;
    g_entityManager->Get<Transform>(3).orientation = 0.f;
    g_entityManager->Get<RigidBody>(3).SetType(EnumRigidBodyType::STATIC);
    g_entityManager->Get<Collider>(3).colliderVariant = AABBCollider();
    
    g_entityManager->Get<Transform>(4).position.x = 0.f;
    g_entityManager->Get<Transform>(4).position.y = -350.f;
    g_entityManager->Get<Transform>(4).width = 1500.f;
    g_entityManager->Get<Transform>(4).height = 100.f;
    g_entityManager->Get<Transform>(4).orientation = 0.f;
    g_entityManager->Get<RigidBody>(4).SetType(EnumRigidBodyType::STATIC);
    g_entityManager->Get<Collider>(4).colliderVariant = AABBCollider();

    // AABB Static Collider
    g_entityManager->Get<Transform>(5).position.x = 200.f;
    g_entityManager->Get<Transform>(5).position.y = 200.f;
    g_entityManager->Get<Transform>(5).width = 100.f;
    g_entityManager->Get<Transform>(5).height = 100.f;
    g_entityManager->Get<Transform>(5).orientation = 0.f;
    g_entityManager->Get<RigidBody>(5).SetType(EnumRigidBodyType::STATIC);
    g_entityManager->Get<Collider>(5).colliderVariant = AABBCollider();
    
    // AABB Dynamic Collider
    g_entityManager->Get<Transform>(6).position.x = -200.f;
    g_entityManager->Get<Transform>(6).position.y = 200.f;
    g_entityManager->Get<Transform>(6).width = 180.f;
    g_entityManager->Get<Transform>(6).height = 100.f;
    g_entityManager->Get<Transform>(6).orientation = 0.f;
    g_entityManager->Get<RigidBody>(6).SetType(EnumRigidBodyType::DYNAMIC);
    g_entityManager->Get<Collider>(6).colliderVariant = AABBCollider();

    // Circle Static Collider
    g_entityManager->Get<Transform>(7).position.x = -200.f;
    g_entityManager->Get<Transform>(7).position.y = -200.f;
    g_entityManager->Get<Transform>(7).width = 100.f;
    g_entityManager->Get<Transform>(7).height = 100.f;
    g_entityManager->Get<Transform>(7).orientation = 0.f;
    g_entityManager->Get<RigidBody>(7).SetType(EnumRigidBodyType::STATIC);
    g_entityManager->Get<Collider>(7).colliderVariant = CircleCollider();

    // Circle Dynamic Collider
    g_entityManager->Get<Transform>(8).position.x = 200.f;
    g_entityManager->Get<Transform>(8).position.y = -200.f;
    g_entityManager->Get<Transform>(8).width = 100.f;
    g_entityManager->Get<Transform>(8).height = 100.f;
    g_entityManager->Get<Transform>(8).orientation = 0.f;
    g_entityManager->Get<RigidBody>(8).SetType(EnumRigidBodyType::DYNAMIC);
    g_entityManager->Get<Collider>(8).colliderVariant = CircleCollider();
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
    while (!glfwWindowShouldClose(m_window))            // Continue until the GLFW window is flagged to close
    {
        // Time start
        TimeManager::GetInstance().StartFrame();
        engine_logger.SetTime();
        MemoryManager::GetInstance()->CheckMemoryOver();
        // UPDATE -----------------------------------------------------
        

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
        if (glfwGetKey(m_window, GLFW_KEY_R) == GLFW_PRESS)
        {
            //m_rendererManager->m_mainCamera.AdjustRotationDegrees(1.f);
            // EntityID id = g_entityFactory->CreateFromPrefab("GameObject");

        }

        

        //Audio Stuff - HANS
        AudioManager::GetInstance()->Update();

        if (glfwGetKey(m_window, GLFW_KEY_Q) == GLFW_PRESS)
        {
            EntityID id = g_entityFactory->CreateFromPrefab("GameObject");
            if (std::rand() % 2)
                g_entityManager->Get<Collider>(id).colliderVariant = CircleCollider();
            else
                g_entityManager->Get<Collider>(id).colliderVariant = AABBCollider();
            g_entityManager->Get<Transform>(id).height = 100.f;
            g_entityManager->Get<Transform>(id).width = 100.f;
            g_entityManager->Get<RigidBody>(id).SetType(EnumRigidBodyType::DYNAMIC);
            g_entityManager->Get<Transform>(id).position = vec2{ 0.f, 0.f };
            lastEnt.emplace(id);
        }

        if (glfwGetKey(m_window, GLFW_KEY_E) == GLFW_PRESS)
        {
            if (lastEnt.size())
            {
                g_entityManager->RemoveEntity(lastEnt.front());
                lastEnt.pop();
            }
        }

        if (glfwGetKey(m_window, GLFW_KEY_T) == GLFW_PRESS)
        {
            if (g_entityManager->Get<RigidBody>(0).m_velocity.Dot(g_entityManager->Get<RigidBody>(0).m_velocity) == 0.f)
                g_entityManager->Get<RigidBody>(0).m_velocity = vec2{ 1.f, 0.f };
            g_entityManager->Get<RigidBody>(0).ApplyLinearImpulse(g_entityManager->Get<RigidBody>(0).m_velocity.GetNormalized() * 1000.f);
        }

        if (glfwGetKey(m_window, GLFW_KEY_W) == GLFW_PRESS)
        {
            g_entityManager->Get<RigidBody>(0).ApplyForce(vec2{ 0.f,1.f } * 5000.f);
        }

        if (glfwGetKey(m_window, GLFW_KEY_S) == GLFW_PRESS)
        {
            g_entityManager->Get<RigidBody>(0).ApplyForce(vec2{ 0.f,-1.f }*5000.f);
        }

        if (glfwGetKey(m_window, GLFW_KEY_A) == GLFW_PRESS)
        {
            g_entityManager->Get<RigidBody>(0).ApplyForce(vec2{ -1.f,0.f }*5000.f);
        }

        if (glfwGetKey(m_window, GLFW_KEY_D) == GLFW_PRESS)
        {
            g_entityManager->Get<RigidBody>(0).ApplyForce(vec2{ 1.f,0.f }*5000.f);
        }

        // Physics test
        PhysicsManager::Step(TimeManager::GetInstance().GetDeltaTime());
        CollisionManager::UpdateColliders();
        CollisionManager::TestColliders();
        CollisionManager::ResolveCollision();

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
            TimeManager::GetInstance().SystemStartFrame(i);
            m_systemList[i]->UpdateSystem(TimeManager::GetInstance().GetDeltaTime()); //@TODO: Update delta time value here!!!!!!!!!!!!!!!!!!!!!!!!!!!
            TimeManager::GetInstance().SystemEndFrame(i);
        }

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
    ResourceManager::UnloadResources();
    ResourceManager::DeleteInstance();
    PhysicsManager::DeleteInstance();
    CollisionManager::DeleteInstance();
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
