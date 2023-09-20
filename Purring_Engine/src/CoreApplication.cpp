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

// Audio Stuff - HANS
#include "AudioManager.h"

// testing
Logger engine_logger = Logger("ENGINE");


/*-----------------------------------------------------------------------------
/// <summary>
/// Constructor for the CoreApplication class.
/// Initializes variables and sets up the application window, FPS controller,
/// logging, and rendering system.
/// </summary>
----------------------------------------------------------------------------- */
PE::CoreApplication::CoreApplication()
{
	m_Running = true;
	m_lastFrameTime = 0;

    // Create and set up the window using WindowManager
    m_window = m_windowManager.InitWindow(1000, 1000, "Purring_Engine");

    m_fpsController.SetTargetFPS(60);                   // Default to 60 FPS
    // set flags
    engine_logger.SetFlag(Logger::EnumLoggerFlags::WRITE_TO_CONSOLE | Logger::EnumLoggerFlags::DEBUG, true);
    engine_logger.SetTime();
    engine_logger.AddLog(false, "Engine initialized!", __FUNCTION__);


    // Pass the pointer to the GLFW window to the rendererManager

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
    // std::cout << "test" << std::endl;                // Debug to test msg to console

    // Main Application Loop
    while (!glfwWindowShouldClose(m_window))            // Continue until the GLFW window is flagged to close
    {
        m_fpsController.StartFrame();
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

        //Audio Stuff - HANS
        AudioManager::GetInstance()->Update();

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
            // Provide an accurate delta time for the UpdateSystem function
            m_systemList[i]->UpdateSystem(1.f);
        }

        // Flush log entries
        engine_logger.FlushLog();

        // Finalize FPS calculations for the current frame
        m_fpsController.EndFrame();
    }

    // Cleanup for ImGui
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    // Additional Cleanup (if required)
    m_windowManager.Cleanup();
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
