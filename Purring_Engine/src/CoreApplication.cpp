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
#include "WindowSystem.h"
#include "Logging/Logger.h"

// testing
Logger engine_logger = Logger("ENGINE");

PE::CoreApplication::CoreApplication()
{
	m_Running = true;
	m_lastFrameTime = 0;

    // Create and set up the window using WindowManager
    m_window = m_windowManager.InitWindow(1000, 1000, "Purring_Engine");

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


    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    ImGuiIO& io = ImGui::GetIO();
    io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
    io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;

    ImGui_ImplGlfw_InitForOpenGL(m_window, true);

    ImGui_ImplOpenGL3_Init("#version 460");
}

PE::CoreApplication::~CoreApplication()
{
	// anything for destructor to do?
}

void PE::CoreApplication::Run()
{
	std::cout << "test" << std::endl;
	// main app loop

	//while (m_Running)
	//{

	//}

    // tmp
    while (!glfwWindowShouldClose(m_window))
    {
        engine_logger.SetTime();

        // UPDATE -----------------------------------------------------
        m_fpsController.StartFrame();

        // List of keys to check
        const int keys[] = { GLFW_KEY_1, GLFW_KEY_2, GLFW_KEY_3, GLFW_KEY_4, GLFW_KEY_5, GLFW_KEY_6, GLFW_KEY_7, GLFW_KEY_8 };

        for (int key : keys)
        {
            if (glfwGetKey(m_window, key) == GLFW_PRESS)
            {
                m_fpsController.UpdateTargetFPSBasedOnKey(key);
            }
        }


        //////////////////////////////////////////////////////////////////////////
        //Chunk of IMGUI code needed for all the imgui windows to start
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        int width, height;
        glfwGetWindowSize(m_window, &width, &height);

        io.DisplaySize = ImVec2(width, height);

        float time = (float)glfwGetTime();
        io.DeltaTime = m_time > 0.0f ? (time - m_time) : (1.0f / 60.0f);
        m_time = time;

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        //////////////////////////////////////////////////////////////////////////
        //-----------------------------------------------------------------

        // DRAW -----------------------------------------------------
            // Render scene (placeholder: clear screen)
        glClear(GL_COLOR_BUFFER_BIT);


        ImGuiWindow::GetInstance()->Render();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Swap front and back buffers
        glfwSwapBuffers(m_window);
        // ----------------------------------------------------------


        // engine_logger.AddLog(false, "Frame rendered", __FUNCTION__);
        // Update the title to show FPS (every second in this example)
        double currentTime = glfwGetTime();
        if (currentTime - m_lastFrameTime >= 1.0)
        {
            m_windowManager.UpdateTitle(m_window, m_fpsController.GetFPS());
            m_lastFrameTime = currentTime;
        }

        m_fpsController.EndFrame();
        engine_logger.FlushLog();
        // Poll for and process events
        glfwPollEvents();

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

void PE::CoreApplication::Initialize()
{
	// init systems
}

void PE::CoreApplication::AddSystem(System* system)
{

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