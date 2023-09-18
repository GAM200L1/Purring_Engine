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
    PE::g_entityManager->Get<Transform>(id).scale = vec2{ 100.f, 100.f };
    PE::g_entityManager->Get<Transform>(id2).scale = vec2{ 100.f, 100.f };


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
	std::cout << "test" << std::endl;
	// main app loop

    while (!glfwWindowShouldClose(m_window))
    {
        m_fpsController.StartFrame();
        engine_logger.SetTime();

        // UPDATE -----------------------------------------------------
        

        // List of keys to check
        const int keys[] = { GLFW_KEY_1, GLFW_KEY_2, GLFW_KEY_3, GLFW_KEY_4, GLFW_KEY_5, GLFW_KEY_6, GLFW_KEY_7, GLFW_KEY_8 };

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
            m_rendererManager->m_mainCamera.AdjustMagnification(0.1f);
        }

        if (glfwGetKey(m_window, GLFW_KEY_W) == GLFW_PRESS)
        {
            m_rendererManager->m_mainCamera.AdjustPosition(0.f, 10.f);
        }

        if (glfwGetKey(m_window, GLFW_KEY_S) == GLFW_PRESS)
        {
            m_rendererManager->m_mainCamera.AdjustPosition(0.f, -10.f);
        }

        if (glfwGetKey(m_window, GLFW_KEY_A) == GLFW_PRESS)
        {
            m_rendererManager->m_mainCamera.AdjustPosition(-10.f, 0.f);
        }

        if (glfwGetKey(m_window, GLFW_KEY_D) == GLFW_PRESS)
        {
            m_rendererManager->m_mainCamera.AdjustPosition(10.f, 0.f);
        }
        // PhysicsManager::UpdateDynamics(deltaTime)
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
            m_systemList[i]->UpdateSystem(1.f); //@TODO: Update delta time value here!!!!!!!!!!!!!!!!!!!!!!!!!!!
        }

        engine_logger.FlushLog();

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