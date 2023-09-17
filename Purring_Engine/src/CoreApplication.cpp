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

PE::CoreApplication::CoreApplication()
{
	m_Running = true;
	m_lastFrameTime = 0;

    // Create and set up the window using WindowManager
    m_window = m_windowManager.InitWindow(1000, 1000, "Purring_Engine");

    m_fpsController.SetTargetFPS(60);  // Default to 60 FPS
    // set flags
    engine_logger.SetFlag(Logger::EnumLoggerFlags::WRITE_TO_CONSOLE | Logger::EnumLoggerFlags::DEBUG, true);
    engine_logger.SetTime();
    engine_logger.AddLog(false, "Engine initialized!", __FUNCTION__);


    // Pass the pointer to the GLFW window to the rendererManager
    Graphics::RendererManager* rendererManager{ new Graphics::RendererManager{m_window} };
    AddSystem(rendererManager);

    // Audio Stuff - HANS
    m_audioManager.Init();
    {
        engine_logger.AddLog(false, "Failed to initialize AudioManager", __FUNCTION__);
    }

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
        m_audioManager.Update();

        const int audioKeys[] = { GLFW_KEY_A, GLFW_KEY_S };
        for (int key : audioKeys)
        {
            if (glfwGetKey(m_window, key) == GLFW_PRESS)
            {
                if (key == GLFW_KEY_A)
                {
                    std::cout << "A key pressed\n";
                    m_audioManager.PlaySound("Assets/Audio/sound1.wav");
                }
                else if (key == GLFW_KEY_S)
                {
                    std::cout << "S key pressed\n";
                    m_audioManager.PlaySound("Assets/Audio/sound2.wav");
                }
            }
        }





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