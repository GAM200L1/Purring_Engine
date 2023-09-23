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
    REGISTERCOMPONENT(RigidBody, sizeof(RigidBody));
    REGISTERCOMPONENT(Collider, sizeof(Collider));
    REGISTERCOMPONENT(Transform, sizeof(Transform));
    REGISTERCOMPONENT(Graphics::Renderer, sizeof(Graphics::Renderer));
    //REGISTERCOMPONENT(PlayerStats, sizeof(PlayerStats));
    //EntityID id = g_entityFactory->CreateEntity();
    //EntityID id2 = g_entityFactory->CreateEntity();
    //PE::g_entityFactory->Assign(id, { "RigidBody", "Collider", "Transform"});
    //PE::g_entityFactory->Assign(id2, { "RigidBody", "Transform"});
    //Collider tmp;
    //tmp.colliderVariant = CircleCollider();
    //PE::g_entityFactory->Copy(id, tmp);
    //PE::g_entityFactory->Copy(id, RigidBody());
    //PE::g_entityFactory->Copy(id2, RigidBody());
    //PE::g_entityFactory->Copy(id, Transform());
    //PE::g_entityFactory->Copy(id2, Transform());
    ////PE::g_entityFactory->Copy(id2, tmp2);

    //PE::g_entityManager->Get<Transform>(id).position = vec2{ 50.f, 50.f };
    //PE::g_entityManager->Get<Transform>(id2).position = vec2{ 50.f, 50.f };


    //EntityID id3 = PE::g_entityFactory->CreateFromPrefab("GameObject");
    //Collider col;
    //col.colliderVariant = AABBCollider();
    //col.objectsCollided.emplace(1);
    //PE::g_entityFactory->LoadComponent(id3, "Collider", static_cast<void*>(&col));

    //std::cout << PE::g_entityManager->Get<Transform>(id3).position.x << std::endl;
    //std::cout << PE::g_entityManager->Get<Collider>(id3).objectsCollided.size() << std::endl;


    for (size_t i{}; i < 50; ++i)
    {
        EntityID id = g_entityFactory->CreateFromPrefab("GameObject");
        g_entityManager->Get<Collider>(id).colliderVariant = CircleCollider();
    }
    g_entityManager->Get<Transform>(0).position.x = 100;
    g_entityManager->Get<RigidBody>(0).SetType(EnumRigidBodyType::DYNAMIC);
    //PE::g_entityManager->Get<Collider>(5002).objectsCollided.emplace(1);
    //PE::g_entityManager->Get<Collider>(5002).colliderVariant = AABBCollider();

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
            EntityID id = g_entityFactory->CreateFromPrefab("GameObject");

        }

        

        //Audio Stuff - HANS
        AudioManager::GetInstance()->Update();

        if (glfwGetKey(m_window, GLFW_KEY_Q) == GLFW_PRESS)
        {
           
        }

        if (glfwGetKey(m_window, GLFW_KEY_E) == GLFW_PRESS)
        {
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
        CollisionManager::ResolveCollision(TimeManager::GetInstance().GetDeltaTime());

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
