#include "prpch.h"

// imgui
#include "Imgui/ImGuiWindow.h"
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

// graphics
#include "Graphics/GLHeaders.h"

#include "CoreApplication.h"
#include "WindowManager.h"
#include "Logging/Logger.h"

// Resource manager
#include "ResourceManager/ResourceManager.h"

// Audio Stuff - HANS
#include "AudioManager.h"

// Time
#include "Time/TimeManager.h"

// testing
Logger engine_logger = Logger("ENGINE");
#include "Physics/RigidBody.h"
#include "Physics/Colliders.h"
#include "Physics/CollisionManager.h"
#include "Physics/PhysicsManager.h"
#include "ECS//EntityFactory.h"
#include "ECS/Entity.h"
#include "ECS/Components.h"

PE::EntityManager entityManager;
PE::EntityFactory entityFactory;

PE::CoreApplication::CoreApplication()
{
    REGISTERCOMPONENT(RigidBody, sizeof(RigidBody));
    REGISTERCOMPONENT(Collider, sizeof(Collider));
    REGISTERCOMPONENT(Transform, sizeof(Transform));
    EntityID id = g_entityFactory->CreateEntity();
    EntityID id2 = g_entityFactory->CreateEntity();
    PE::g_entityFactory->Assign(id, { "RigidBody", "Collider", "Transform" });
    PE::g_entityFactory->Assign(id2, { "RigidBody", "Collider", "Transform" });
    Collider tmp, tmp2;
    tmp.colliderVariant = AABBCollider();
    tmp2.colliderVariant = CircleCollider();
    PE::g_entityFactory->Copy(id, tmp);
    PE::g_entityFactory->Copy(id2, tmp2);
    PE::g_entityManager->Get<Transform>(id).position = vec2{ 40.f, 84.f };
    PE::g_entityManager->Get<Transform>(id2).position = vec2{ 130.f, 50.f };
    PE::g_entityManager->Get<Transform>(id).angle = 0.f;
    PE::g_entityManager->Get<Transform>(id).scale = vec2{ 100.f, 100.f };
    PE::g_entityManager->Get<Transform>(id2).scale = vec2{ 100.f, 100.f };
    PE::g_entityManager->Get<RigidBody>(id).SetType(EnumRigidBodyType::DYNAMIC);
    PE::g_entityManager->Get<RigidBody>(id).m_velocity = vec2{ 0.f, 0.f };
    PE::g_entityManager->Get<RigidBody>(id).m_force = vec2{ 0.f, 0.f };
    PE::g_entityManager->Get<RigidBody>(id).SetMass(10.f);



    m_Running = true;
    m_lastFrameTime = 0;
    m_Running = true;
    m_lastFrameTime = 0.0;

    // Create and set up the window using WindowManager
    m_window = m_windowManager.InitWindow(1000, 1000, "Engine");

    m_fpsController.SetTargetFPS(60);  // Default to 60 FPS
    // set flags
    engine_logger.SetFlag(Logger::EnumLoggerFlags::WRITE_TO_CONSOLE | Logger::EnumLoggerFlags::DEBUG, true);
    engine_logger.SetTime();
    engine_logger.AddLog(false, "Engine initialized!", __FUNCTION__);

    // Pass the pointer to the GLFW window to the rendererManager
    m_rendererManager = new Graphics::RendererManager{ m_window };
    AddSystem(m_rendererManager);
}
    // Audio Stuff - HANS
    /*m_audioManager.Init();
    {
        engine_logger.AddLog(false, "Failed to initialize AudioManager", __FUNCTION__);
    }*/

PE::CoreApplication::~CoreApplication()
{
	// anything for destructor to do?
}

void PE::CoreApplication::Run()
{
    TimeManager::GetInstance().EngineStart();
	// main app loop

    while (!glfwWindowShouldClose(m_window))
    {
        // time start
        TimeManager::GetInstance().StartFrame();
        engine_logger.SetTime();

        //std::cout << TimeManager::GetInstance().GetRunTime() << " Delta Time: " << TimeManager::GetInstance().GetDeltaTime() << std::endl;

        // UPDATE -----------------------------------------------------
        

        // List of keys to check
        const int keys[] = { GLFW_KEY_1, GLFW_KEY_2, GLFW_KEY_3, GLFW_KEY_4, GLFW_KEY_5, GLFW_KEY_6, GLFW_KEY_7, GLFW_KEY_8 };
        RigidBody& rb = g_entityManager->Get<RigidBody>(0);
        for (int key : keys)
        {
            if (glfwGetKey(m_window, key) == GLFW_PRESS)
            {
                m_fpsController.UpdateTargetFPSBasedOnKey(key);
            }
        }
        if (glfwGetKey(m_window, GLFW_KEY_R) == GLFW_PRESS)
        {
            m_rendererManager->m_mainCamera.AdjustRotationDegrees(1.f);
        }

        if (glfwGetKey(m_window, GLFW_KEY_T) == GLFW_PRESS)
        {
            m_rendererManager->m_mainCamera.AdjustRotationDegrees(-1.f);
        }

        if (glfwGetKey(m_window, GLFW_KEY_Q) == GLFW_PRESS)
        {
            m_rendererManager->m_mainCamera.AdjustMagnification(-0.1f);
        }

        if (glfwGetKey(m_window, GLFW_KEY_E) == GLFW_PRESS)
        {
            //m_rendererManager->m_mainCamera.AdjustMagnification(0.1f);
            if (rb.m_velocity.Dot(rb.m_velocity) == 0.f)
                rb.m_velocity = vec2{ 1.f, 0.f };
            rb.ApplyLinearImpulse(rb.m_velocity.GetNormalized() * 1000.f);
        }
        float force = 5000.f;
        if (glfwGetKey(m_window, GLFW_KEY_W) == GLFW_PRESS)
        {
            //m_rendererManager->m_mainCamera.AdjustPosition(0.f, 10.f);
            //if (rb.GetType() == EnumRigidBodyType::DYNAMIC)
                rb.ApplyForce(vec2{ 0.f, 1.f } * force);
            //else
               // rb.m_velocity += vec2{ 0.f, 50.f };
        }

        if (glfwGetKey(m_window, GLFW_KEY_S) == GLFW_PRESS)
        {
            //m_rendererManager->m_mainCamera.AdjustPosition(0.f, -10.f);
            //if (rb.GetType() == EnumRigidBodyType::DYNAMIC)
                rb.ApplyForce(vec2{ 0.f, -1.f } * force);
            //else
               // rb.m_velocity += vec2{ 0.f, -50.f };
        }

        if (glfwGetKey(m_window, GLFW_KEY_A) == GLFW_PRESS)
        {
            //m_rendererManager->m_mainCamera.AdjustPosition(-10.f, 0.f);
            //if (rb.GetType() == EnumRigidBodyType::DYNAMIC)
                rb.ApplyForce(vec2{ -1.f, 0.f }*force);
            //else
               // rb.m_velocity += vec2{ -50.f, 0.f };
        }

        if (glfwGetKey(m_window, GLFW_KEY_D) == GLFW_PRESS)
        {
            //m_rendererManager->m_mainCamera.AdjustPosition(10.f, 0.f);
            //if (rb.GetType() == EnumRigidBodyType::DYNAMIC)
                rb.ApplyForce(vec2{ 1.f, 0.f } * force);
            //else
                //rb.m_velocity += vec2{ 50.f, 0.f };
        }
        
        PhysicsManager::UpdateDynamics(TimeManager::GetInstance().GetDeltaTime());
        CollisionManager::UpdateColliders();
        CollisionManager::TestColliders();

        /*// Audio Stuff - HANS
        //m_audioManager.Update();

        const int audioKeys[] = { GLFW_KEY_A, GLFW_KEY_S };
        for (int key : audioKeys)
        {
            if (glfwGetKey(m_window, key) == GLFW_PRESS)
            {
                if (key == GLFW_KEY_A)
                {
                    std::cout << "A key pressed\n";
                    m_audioManager.PlaySound("../Assets/Audio/sound1.wav");
                }
                else if (key == GLFW_KEY_S)
                {
                    std::cout << "S key pressed\n";
                    m_audioManager.PlaySound("../Assets/Audio/sound2.wav");
                }
            }
        }*/

        // Update the title to show FPS (every second in this example)
        double currentTime = glfwGetTime();
        if (currentTime - m_lastFrameTime >= 1.0)
        {
            m_windowManager.UpdateTitle(m_window, m_fpsController.GetFPS());
            m_lastFrameTime = currentTime;
        }

        // update systems
        for (unsigned int i{ 0 }; i < m_systemList.size(); ++i)
        {
            TimeManager::GetInstance().SystemStartFrame(i);
            m_systemList[i]->UpdateSystem(TimeManager::GetInstance().GetDeltaTime()); //@TODO: Update delta time value here!!!!!!!!!!!!!!!!!!!!!!!!!!!
            TimeManager::GetInstance().SystemEndFrame(i);
        }

        //-----System profiling to be moved to IMGUI
        // std::cout << "Percentage %: " << TimeManager::GetInstance().GetSystemFrameTime(0) << ", " 
        //           << TimeManager::GetInstance().GetFrameTime() << " | "
        //           << ((TimeManager::GetInstance().GetSystemFrameTime(0) / TimeManager::GetInstance().GetFrameTime()) * 100.f) << "%" << '\n';
        //-----------------------
        
        engine_logger.FlushLog();

        TimeManager::GetInstance().EndFrame();
        m_fpsController.EndFrame();
    }

    // Clean up of imgui functions
    /*ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    // Cleanup (if needed)
    m_windowManager.Cleanup();
    PE::ResourceManager::UnloadResources();*/
}

void PE::CoreApplication::InitSystems()
{
    // init all systems
    for (System* system : m_systemList)
    {
        system->InitializeSystem();
    }
}

void PE::CoreApplication::DestroySystems()
{
    // destroy all systems
    for (System* system : m_systemList)
    {
        system->DestroySystem();
        delete system;
    }
}

void PE::CoreApplication::AddSystem(System* system)
{
    // add system to core application
    m_systemList.push_back(system);
}