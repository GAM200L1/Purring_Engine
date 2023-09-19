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

PE::EntityManager entityManager;
PE::EntityFactory entityFactory;

PE::CoreApplication::CoreApplication()
{
    REGISTERCOMPONENT(RigidBody, sizeof(RigidBody));
    REGISTERCOMPONENT(Collider, sizeof(Collider));
    REGISTERCOMPONENT(Transform, sizeof(Transform));
    REGISTERCOMPONENT(PlayerStats, sizeof(PlayerStats));
    EntityID id = g_entityFactory->CreateEntity();
    EntityID id2 = g_entityFactory->CreateEntity();
    PE::g_entityFactory->Assign(id, { "RigidBody", "Collider", "Transform"});
    PE::g_entityFactory->Assign(id2, { "RigidBody", "Collider", "Transform"});
    Collider tmp, tmp2;
    tmp.colliderVariant = CircleCollider();
    tmp2.colliderVariant = AABBCollider();
    PE::g_entityFactory->Copy(id, tmp);
    PE::g_entityFactory->Copy(id, RigidBody());
    PE::g_entityFactory->Copy(id2, RigidBody());
    PE::g_entityFactory->Copy(id, Transform());
    PE::g_entityFactory->Copy(id2, Transform());
    PE::g_entityFactory->Copy(id2, tmp2);

    PE::g_entityManager->Get<Transform>(id).position = vec2{ 50.f, 50.f };
    PE::g_entityManager->Get<Transform>(id2).position = vec2{ 50.f, 50.f };
    PE::g_entityManager->Get<Transform>(id).scale = vec2{ 10.f, 10.f };
    PE::g_entityManager->Get<Transform>(id2).scale = vec2{ 10.f, 10.f };


    EntityID id3 = PE::g_entityFactory->CreateFromPrefab("GameObject");
    Collider col;
    col.colliderVariant = AABBCollider();
    col.objectsCollided.emplace(1);
    PE::g_entityFactory->LoadComponent(id3, "Collider", static_cast<void*>(&col));

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


    for (EntityID id : SceneView())
    {
        std::vector<ComponentID> components = g_entityManager->GetComponentIDs(id);
        std::cout << "Entity " << id << " Has: \n";
        for (const ComponentID& name : components)
        {
            std::cout << name << ", ";
            if (name == "RigidBody")
            {
                std::cout << "Awake: " << g_entityManager->Get<RigidBody>(id).m_awake << std::endl;
            }
            if (name == "Collider")
            {
                std::cout << "Number of collisions: " <<
                g_entityManager->Get<Collider>(id).objectsCollided.size() << std::endl;
            }
            if (name == "Transform")
            {
                std::cout << "Angle: " << g_entityManager->Get<Transform>(id).position.x << std::endl;
            }
        }
        std::cout << std::endl;
    }

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

        // Just test assigning entity IDs
        int playerEntityId = 5;
        // Assign the entity name for the player
        sm.setEntityName(playerEntityId, "PlayerStatistics");

        // PlayerStats object with some sample data
        PlayerStats myStats;
        myStats.health = 100;
        myStats.level = 5;
        myStats.experience = 24.5;

        // Fetch the entity name if available
        myStats.playerName = sm.getEntityName(playerEntityId);

        // S to serialize
        if (glfwGetKey(m_window, GLFW_KEY_S) == GLFW_PRESS)
        {
            nlohmann::json serializedStats = myStats.to_json(playerEntityId, sm);
            std::ofstream outFile("serializedPlayerStats.json");
            if (outFile.is_open()) {
                outFile << serializedStats.dump(4);
                outFile.close();
                std::cout << "PlayerStats serialized and saved to 'serializedPlayerStats.json'." << std::endl;
            }
        }

        // L to deserialize
        if (glfwGetKey(m_window, GLFW_KEY_L) == GLFW_PRESS)
        {
            std::ifstream inFile("serializedPlayerStats.json");
            if (inFile.is_open()) {
                nlohmann::json j;
                inFile >> j;
                inFile.close();
                myStats.from_json(j, playerEntityId, sm);

                PE::g_entityFactory->LoadComponent(2, "PlayerStats", static_cast<void*>(&myStats));

                // Output all data members of PlayerStats
                std::cout << "Successfully deserialized and loaded PlayerStats:" << std::endl;
                std::cout << "Health: " << PE::g_entityManager->Get<PlayerStats>(2).health << std::endl;
                std::cout << "Level: " << PE::g_entityManager->Get<PlayerStats>(2).level << std::endl;
                std::cout << "Experience: " << PE::g_entityManager->Get<PlayerStats>(2).experience << std::endl;
                std::cout << "Player Name: " << PE::g_entityManager->Get<PlayerStats>(2).playerName << std::endl;
                std::cout << "Entity ID: " << 2 << std::endl;
                std::cout << "Entity Name: " << sm.getEntityName(playerEntityId) << std::endl;
            }
        }

        //// Initialize an Entity and set its name 
        //Entity entity1;
        //entity1.name = "TestEntity1";
        //entity1.data["someInt"] = std::any(42);
        //entity1.data["someFloat"] = std::any(3.14f);
        //entity1.data["someString"] = std::any(std::string("Hello, world!"));
        //entity1.data["someIntArray"] = std::any(std::vector<int>{1, 2, 3, 4, 5});

        //// Add the Entity to the SerializationManager
        //sm.setEntity(1, entity1);

        //// S to serialize
        //if (glfwGetKey(m_window, GLFW_KEY_S) == GLFW_PRESS)
        //{
        //    sm.saveToFile("serializedEntity.json", 1);
        //    std::cout << "Entity has been serialized and saved to 'serializedEntity.json'." << std::endl;
        //}

        //// L to deserialize
        //if (glfwGetKey(m_window, GLFW_KEY_L) == GLFW_PRESS)
        //{
        //    try {
        //        std::pair<Entity, int> loadedData = sm.loadFromFile("serializedEntity.json");
        //        Entity loadedEntity = loadedData.first;
        //        int entityID = loadedData.second;

        //        if (entityID != -1) {
        //            std::cout << std::any_cast<int>(loadedEntity.data["someInt"]) << std::endl;
        //            std::cout << std::any_cast<float > (loadedEntity.data["someFloat"]) << std::endl;
        //            std::cout << std::any_cast<std::string> (loadedEntity.data["someString"]) << std::endl;
        //            std::cout << "Successfully loaded entity with ID: " << entityID << std::endl;
        //        }
        //        else {
        //            std::cout << "Failed to load entity from file." << std::endl;
        //        }
        //    }
        //    catch (const std::exception& e) {
        //        std::cerr << "Exception caught: " << e.what() << std::endl;
        //    }
        //}


        //// Initialize newEntity with new fields
        //Entity newEntity;
        //newEntity.id = 5;
        //newEntity.someInt = 42;
        //newEntity.someFloat = 3.14f;
        //newEntity.someDouble = 2.71828;
        //newEntity.someChar = 'A';
        //newEntity.someBool = true;
        //newEntity.someString = "Hello, world!";

        //// Set entity in manager
        //manager.setEntity(5, newEntity);

        //// Just for example, let's assume you press the 'S' key to serialize data
        //if (glfwGetKey(m_window, GLFW_KEY_S) == GLFW_PRESS)
        //{
        //    // Serialize data
        //    manager.saveToFile("SavedFile.json", 5);
        //}

        //// Let's assume you press the 'L' key to load serialized data
        //if (glfwGetKey(m_window, GLFW_KEY_L) == GLFW_PRESS)
        //{
        //    // Deserialize data
        //    auto [loadedEntity, loadedID] = manager.loadFromFile("SavedFile.json");
        //    // Now loadedEntity contains the deserialized data, use it as needed
        //}






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