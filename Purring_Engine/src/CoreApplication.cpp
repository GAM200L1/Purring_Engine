#include "prpch.h"

// imgui
#include "Imgui/ImGuiWindow.h"
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

// graphics
#define GLEW_STATIC
#include "GL/glew.h"
#include "GLFW/glfw3.h"

#include "CoreApplication.h"
#include "WindowManager.h"
#include "Logging/Logger.h"

#include "Data/SerializationManager.h"


// testing
Logger engine_logger = Logger("ENGINE");
// Create an instance of the SerializationManager class.
SerializationManager sm;

PE::CoreApplication::CoreApplication()
{
	m_Running = true;
	m_lastFrameTime = 0;

    // Create and set up the window using WindowManager
    m_window = m_windowManager.InitWindow(1000, 1000, "Engine");

    // Initialize GLEW
    if (glewInit() != GLEW_OK)
    {
        std::cerr << "Failed to initialize GLEW." << std::endl;
        exit(-1);
    }

    PrintSpecs();

    m_fpsController.SetTargetFPS(60);  // Default to 60 FPS
    // set flags
    engine_logger.SetFlag(Logger::EnumLoggerFlags::WRITE_TO_CONSOLE | Logger::EnumLoggerFlags::DEBUG, true);
    engine_logger.SetTime();
    engine_logger.AddLog(false, "Engine initialized!", __FUNCTION__);

    //init imgui settings
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    ImGuiIO& io = ImGui::GetIO();
    io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
    io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;

    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;


    ///////////////////////////////////////////
    //temp here untill i can get window exposed
    int width, height;
    glfwGetWindowSize(m_window, &width, &height);
    io.DisplaySize = ImVec2(width, height);

    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    ImGui_ImplGlfw_InitForOpenGL(m_window, true);

    ImGui_ImplOpenGL3_Init("#version 460");
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

                // Output all data members of PlayerStats
                std::cout << "Successfully deserialized PlayerStats:" << std::endl;
                std::cout << "Health: " << myStats.health << std::endl;
                std::cout << "Level: " << myStats.level << std::endl;
                std::cout << "Experience: " << myStats.experience << std::endl;
                std::cout << "Player Name: " << myStats.playerName << std::endl;
                std::cout << "Entity ID: " << playerEntityId << std::endl;
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
        glClear(GL_COLOR_BUFFER_BIT);

        //////////////////////////////////////////////////////////////////////////
        //temp here untill window is exposed
        ImGuiIO& io = ImGui::GetIO();
        float time = (float)glfwGetTime();
        io.DeltaTime = m_time > 0.0f ? (time - m_time) : (1.0f / 60.0f);
        m_time = time;

        //redering of all windows
        ImGuiWindow::GetInstance()->Render();
        //////////////////////////////////////////////////////////////////////
        // 
        // Swap front and back buffers
        glfwSwapBuffers(m_window);
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
            m_systemList[i]->UpdateSystem();
        }

        engine_logger.FlushLog();

        // Poll for and process events
        glfwPollEvents(); // should be called before glfwSwapbuffers
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
        system->InitSystem();
    }
}

void PE::CoreApplication::DestroySystems()
{
    // destroy all systems
    for (System* system : m_systemList)
    {
        delete system;
    }
}

void PE::CoreApplication::AddSystem(System* system)
{
    // add system to core application
    m_systemList.push_back(system);

}

void PE::CoreApplication::PrintSpecs()
{
    // Declare variables to store specs info
    GLint majorVersion, minorVersion, maxVertexCount, maxIndicesCount, maxTextureSize, maxViewportDims[2];
    GLboolean isdoubleBuffered;

    // Get and store values
    glGetIntegerv(GL_MAJOR_VERSION, &majorVersion);
    glGetIntegerv(GL_MINOR_VERSION, &minorVersion);
    glGetIntegerv(GL_MAX_ELEMENTS_VERTICES, &maxVertexCount);
    glGetIntegerv(GL_MAX_ELEMENTS_INDICES, &maxIndicesCount);
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);
    glGetIntegerv(GL_MAX_VIEWPORT_DIMS, maxViewportDims);
    glGetBooleanv(GL_DOUBLEBUFFER, &isdoubleBuffered);

    // Print out specs
    std::cout << "GPU Vendor: " << glGetString(GL_VENDOR)
        << "\nGL Renderer: " << glGetString(GL_RENDERER)
        << "\nGL Version: " << glGetString(GL_VERSION)
        << "\nGL Shader Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION)
        << "\nGL Major Version: " << majorVersion
        << "\nGL Minor Version: " << minorVersion
        << "\nCurrent OpenGL Context is " << (isdoubleBuffered ? "double buffered" : "single buffered")
        << "\nMaximum Vertex Count: " << maxVertexCount
        << "\nMaximum Indices Count: " << maxIndicesCount
        << "\nGL Maximum texture size: " << maxTextureSize
        << "\nMaximum Viewport Dimensions: " << maxViewportDims[0] << " x " << maxViewportDims[1] << "\n" << std::endl;
}