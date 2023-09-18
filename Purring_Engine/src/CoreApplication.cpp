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
#include "ECS/Prefabs.h"

PE::EntityManager entityManager;
PE::EntityFactory entityFactory;

PE::CoreApplication::CoreApplication()
{
    REGISTERCOMPONENT(RigidBody, sizeof(RigidBody));
    REGISTERCOMPONENT(Collider, sizeof(Collider));
    REGISTERCOMPONENT(Transform, sizeof(Transform));
    EntityID id = g_entityFactory->CreateEntity();
    EntityID id2 = g_entityFactory->CreateEntity();
    PE::g_entityFactory->Assign(id, { "RigidBody", "Collider", "Transform"});
    PE::g_entityFactory->Assign(id2, { "RigidBody", "Collider", "Transform"});
    Collider tmp, tmp2;
    tmp.colliderVariant = CircleCollider();
    tmp2.colliderVariant = AABBCollider();
    PE::g_entityFactory->Copy(id, tmp);
    PE::g_entityFactory->Copy(id2, tmp2);
    PE::g_entityManager->Get<Transform>(id).position = vec2{ 50.f, 50.f };
    PE::g_entityManager->Get<Transform>(id2).position = vec2{ 50.f, 50.f };
    PE::g_entityManager->Get<Transform>(id).scale = vec2{ 10.f, 10.f };
    PE::g_entityManager->Get<Transform>(id2).scale = vec2{ 10.f, 10.f };

    PE::LoadPrefabs();

    EntityID id3 = PE::CreatePrefab("GameObject");
    Collider col;
    col.colliderVariant = AABBCollider();
    col.objectsCollided.emplace(1);
    PE::InitializeCollider(id3, static_cast<void*>(&col));

    std::cout << PE::g_entityManager->Get<Transform>(id3).position.x << std::endl;
    std::cout << PE::g_entityManager->Get<Collider>(id3).objectsCollided.size() << std::endl;

	m_Running = true;
	m_lastFrameTime = 0;

    // Create and set up the window using WindowManager
    m_window = m_windowManager.InitWindow(1000, 1000, "Engine");

    m_fpsController.SetTargetFPS(60);  // Default to 60 FPS
    // set flags
    engine_logger.SetFlag(Logger::EnumLoggerFlags::WRITE_TO_CONSOLE | Logger::EnumLoggerFlags::DEBUG, true);
    engine_logger.SetTime();
    engine_logger.AddLog(false, "Engine initialized!", __FUNCTION__);

    // Pass the pointer to the GLFW window to the rendererManager
    Graphics::RendererManager* rendererManager{ new Graphics::RendererManager{m_window} };
    AddSystem(rendererManager);

    // Audio Stuff - HANS
    /*m_audioManager.Init();
    {
        engine_logger.AddLog(false, "Failed to initialize AudioManager", __FUNCTION__);
    }*/

    //init imgui settings
    //IMGUI_CHECKVERSION();
    //ImGui::CreateContext();
    //ImGui::StyleColorsDark();

    //ImGuiIO& io = ImGui::GetIO();
    //io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
    //io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;

    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    //io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    //io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;


    ///////////////////////////////////////////
    //temp here untill i can get window exposed
    //int width, height;
    //glfwGetWindowSize(m_window, &width, &height);
    //io.DisplaySize = ImVec2(width, height);

    //ImGuiStyle& style = ImGui::GetStyle();
    //if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
    //    style.WindowRounding = 0.0f;
    //    style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    //}

    //ImGui_ImplGlfw_InitForOpenGL(m_window, true);

    //ImGui_ImplOpenGL3_Init("#version 460");
    ///////////////////////////////////////////
}

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

        // Audio Stuff - HANS
        //m_audioManager.Update();

        const int audioKeys[] = { GLFW_KEY_A, GLFW_KEY_S };
        for (int key : audioKeys)
        {
            if (glfwGetKey(m_window, key) == GLFW_PRESS)
            {
                if (key == GLFW_KEY_A)
                {
                    std::cout << "A key pressed\n";
                    //m_audioManager.PlaySound("Audio/sound1.wav");
                }
                else if (key == GLFW_KEY_S)
                {
                    std::cout << "S key pressed\n";
                    //m_audioManager.PlaySound("Audio/sound2.wav");
                }
            }
        }


        // Physics test
        //PhysicsManager::UpdateDynamics(60.f);
        CollisionManager::TestColliders();
        CollisionManager::UpdateColliders();


        // DRAW -----------------------------------------------------
            // Render scene (placeholder: clear screen)
        //glClear(GL_COLOR_BUFFER_BIT);

        //////////////////////////////////////////////////////////////////////////
        //temp here untill window is exposed
        //ImGuiIO& io = ImGui::GetIO();
        //float time = (float)glfwGetTime();
        //io.DeltaTime = m_time > 0.0f ? (time - m_time) : (1.0f / 60.0f);
        //m_time = time;

        ////redering of all windows
        //ImGuiWindow::GetInstance()->Render();
        //////////////////////////////////////////////////////////////////////
        // 
        // Swap front and back buffers
        //glfwSwapBuffers(m_window);
        // DRAW ----------------------------------------------------------


        // engine_logger.AddLog(false, "Frame rendered", __FUNCTION__);
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

    /// <summary>
    /// Clean up of imgui functions
    /// </summary>
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    // Cleanup (if needed)
    m_windowManager.Cleanup();
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