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
#include <random>

PE::EntityManager entManager;
PE::EntityFactory entFactory;

std::queue<EntityID> lastEnt{};

std::vector<EntityID> testVector;


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
    //assignning memory manually to renderer manager

    // Add system to list
    Graphics::RendererManager* rendererManager = new (MemoryManager::GetInstance().AllocateMemory("Graphics Manager", sizeof(Graphics::RendererManager)))Graphics::RendererManager{m_window};
    PhysicsManager* physicsManager = new (MemoryManager::GetInstance().AllocateMemory("Physics Manager", sizeof(PhysicsManager)))PhysicsManager{};
    CollisionManager* collisionManager = new (MemoryManager::GetInstance().AllocateMemory("Collision Manager", sizeof(CollisionManager)))CollisionManager{};
    InputSystem* ip = new (MemoryManager::GetInstance().AllocateMemory("Input System", sizeof(InputSystem)))InputSystem{};
    AddSystem(ip);
    AddSystem(physicsManager);
    AddSystem(collisionManager);
    AddSystem(rendererManager);


    // Load a texture
    std::string catTextureName{ "cat" };
    ResourceManager::GetInstance().LoadTextureFromFile(catTextureName, "../Assets/Textures/Cat1_128x128.png");
    ResourceManager::GetInstance().LoadTextureFromFile("cat2", "../Assets/Textures/image2.png");

    g_entityFactory->CreateFromPrefab("GameObject");
    g_entityFactory->CreateFromPrefab("GameObject");

    // ----- Limit Test for Physics ----- //
    //std::random_device rd;
    //std::mt19937 gen(rd());
    //for (size_t i{ 2 }; i < 20; ++i)
    //{
    //    EntityID id = g_entityFactory->CreateFromPrefab("GameObject");
    //
    //    std::uniform_int_distribution<>distr0(-550, 550);
    //    g_entityManager->Get<Transform>(id).position.x = static_cast<float>(distr0(gen));
    //    std::uniform_int_distribution<>distr1(-250, 250);
    //    g_entityManager->Get<Transform>(id).position.y = static_cast<float>(distr1(gen));
    //    std::uniform_int_distribution<>distr2(10, 200);
    //    g_entityManager->Get<Transform>(id).width = static_cast<float>(distr2(gen));
    //    g_entityManager->Get<Transform>(id).height = static_cast<float>(distr2(gen));
    //    g_entityManager->Get<Transform>(id).orientation = 0.f;
    //
    //    if (i%3)
    //        g_entityManager->Get<RigidBody>(id).SetType(EnumRigidBodyType::DYNAMIC);
    //    
    //    if (i%2)
    //        g_entityManager->Get<Collider>(id).colliderVariant = CircleCollider();
    //    else
    //        g_entityManager->Get<Collider>(id).colliderVariant = AABBCollider();
    //}

    // Make the first gameobject with a collider circle at world pos (100, 100)
    g_entityManager->Get<Transform>(0).position.x = 0.f;
    g_entityManager->Get<Transform>(0).position.y = 0.f;
    g_entityManager->Get<Transform>(0).width = 100.f;
    g_entityManager->Get<Transform>(0).height = 100.f;
    g_entityManager->Get<Transform>(0).orientation = 0.f;
    g_entityManager->Get<RigidBody>(0).SetType(EnumRigidBodyType::DYNAMIC);
    g_entityManager->Get<Collider>(0).colliderVariant = CircleCollider();
    g_entityManager->Get<Graphics::Renderer>(0).SetTextureKey(catTextureName);
    g_entityManager->Get<Graphics::Renderer>(0).SetColor(1.f, 1.f, 0.f);
    g_entityManager->Get<RigidBody>(0).SetMass(10.f);

    // Make the second gameobject a rectangle with an AABB collider at world pos (-100, -100)
    g_entityManager->Get<Transform>(1).position.x = -100.f;
    g_entityManager->Get<Transform>(1).position.y = -100.f;
    g_entityManager->Get<Transform>(1).width = 50.f;
    g_entityManager->Get<Transform>(1).height = 200.f;
    g_entityManager->Get<Transform>(1).orientation = 0.f;
    g_entityManager->Get<RigidBody>(1).SetType(EnumRigidBodyType::DYNAMIC);
    g_entityManager->Get<Collider>(1).colliderVariant = AABBCollider();
    g_entityManager->Get<Collider>(1).isTrigger = true;

    for (size_t i{}; i < 2500; ++i) {
        EntityID id2 = g_entityFactory->CreateEntity();
        g_entityFactory->Assign(id2, { "Transform", "Renderer" });
        g_entityManager->Get<Transform>(id2).position.x = 50.f * (i % 50) - 200.f;
        g_entityManager->Get<Transform>(id2).position.y = 50.f * (i / 50) - 300.f;
        g_entityManager->Get<Transform>(id2).width = 50.f;
        g_entityManager->Get<Transform>(id2).height = 50.f;
        g_entityManager->Get<Transform>(id2).orientation = 0.f;
        g_entityManager->Get<Graphics::Renderer>(id2).SetTextureKey(catTextureName);
        g_entityManager->Get<Graphics::Renderer>(id2).SetColor(1.f, 0.f, 1.f, 0.1f);
    }
    for (const EntityID& id : SceneView<Graphics::Renderer, Transform>())
    {
        testVector.emplace_back(id);
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
    while (!glfwWindowShouldClose(m_window))            // Continue until the GLFW window is flagged to close
    {
        // Time start
        TimeManager::GetInstance().StartFrame();
        engine_logger.SetTime();
        MemoryManager::GetInstance().CheckMemoryOver();
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
            clock_t start, end;
            start = clock();
            for (const EntityID& id : SceneView<Graphics::Renderer, Transform>())
            {
                id;
            }
            end = clock();
            double total = double(end - start) / double(CLOCKS_PER_SEC);
            std::string str = "SceneView<Renderer, Transform>() took: " + std::to_string(total) + " sec to run...";
            engine_logger.AddLog(false, str, __FUNCTION__);
        }
        if (glfwGetKey(m_window, GLFW_KEY_T) == GLFW_PRESS)
        {
            //m_rendererManager->m_mainCamera.AdjustRotationDegrees(1.f);
            // EntityID id = g_entityFactory->CreateFromPrefab("GameObject");
            clock_t start, end;
            start = clock();
            for (const EntityID& id : SceneView<Graphics::Renderer>())
            {
                id;
            }
            end = clock();
            double total = double(end - start) / double(CLOCKS_PER_SEC);
            std::string str = "SceneView<Renderer>() took: " + std::to_string(total) + " sec to run...";
            engine_logger.AddLog(false, str, __FUNCTION__);
        }
        if (glfwGetKey(m_window, GLFW_KEY_Y) == GLFW_PRESS)
        {
            //m_rendererManager->m_mainCamera.AdjustRotationDegrees(1.f);
            // EntityID id = g_entityFactory->CreateFromPrefab("GameObject");
            clock_t start, end;
            start = clock();
            for (const EntityID& id : testVector)
            {
                id;
            }
            end = clock();
            double total = double(end - start) / double(CLOCKS_PER_SEC);
            std::string str = "Stored vector took: " + std::to_string(total) + " sec to run...";
            engine_logger.AddLog(false, str, __FUNCTION__);
        }

        //Audio Stuff - HANS
        AudioManager::GetInstance()->Update();

        //if (glfwGetKey(m_window, GLFW_KEY_Q) == GLFW_PRESS)
        //{
        //    EntityID id = g_entityFactory->CreateFromPrefab("GameObject");
        //    g_entityManager->Get<Collider>(id).colliderVariant = CircleCollider();
        //    g_entityManager->Get<Transform>(id).height = 100.f;
        //    g_entityManager->Get<Transform>(id).width = 100.f;
        //    g_entityManager->Get<RigidBody>(id).SetType(EnumRigidBodyType::DYNAMIC);
        //    g_entityManager->Get<Transform>(id).position = vec2{ 0.f, 0.f };
        //    lastEnt.emplace(id);
        //}
        //
        //if (glfwGetKey(m_window, GLFW_KEY_E) == GLFW_PRESS)
        //{
        //    if (lastEnt.size())
        //    {
        //        g_entityManager->RemoveEntity(lastEnt.front());
        //        lastEnt.pop();
        //    }
        //}

        //if (glfwGetKey(m_window, GLFW_KEY_L) == GLFW_PRESS)
        //{
        //    EntityManager ent;
        //}

        // Set step physics
        if (glfwGetKey(m_window, GLFW_KEY_P) == GLFW_PRESS)
        {
            PhysicsManager::GetStepPhysics() = !PhysicsManager::GetStepPhysics();
        }
        if (glfwGetKey(m_window, GLFW_KEY_N) == GLFW_PRESS)
        {
            PhysicsManager::GetAdvanceStep() = !PhysicsManager::GetAdvanceStep();
        }

        // Character movement
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

        // dash
        if (glfwGetKey(m_window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        {
            if (g_entityManager->Get<RigidBody>(0).m_velocity.Dot(g_entityManager->Get<RigidBody>(0).m_velocity) == 0.f)
                g_entityManager->Get<RigidBody>(0).m_velocity = vec2{ 1.f, 0.f };
            g_entityManager->Get<RigidBody>(0).ApplyLinearImpulse(g_entityManager->Get<RigidBody>(0).m_velocity.GetNormalized() * 1000.f);
        }

        // Character Rotation
        if (glfwGetKey(m_window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        {
            g_entityManager->Get<RigidBody>(0).m_rotationVelocity = -PE_PI;
        }
        if (glfwGetKey(m_window, GLFW_KEY_LEFT) == GLFW_PRESS)
        {
            g_entityManager->Get<RigidBody>(0).m_rotationVelocity = PE_PI;
        }

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
