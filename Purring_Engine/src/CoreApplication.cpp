/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     CoreApplication.cpp
 \date     28-08-2023

 \author               Brandon HO Jun Jie
 \par      email:      brandonjunjie.ho@digipen.edu

 \brief    This file contains the CoreApplication class, which serves as the entry point for
           the engine. It handles the main application loop, initializes and updates all registered 
           systems, and manages application-level resources such as the window and FPS controller.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/

/*                                                                                                          includes
--------------------------------------------------------------------------------------------------------------------- */
#include "prpch.h"


// Entity Component System (ECS)
#include "ECS/EntityFactory.h"
#include "ECS/Entity.h"
#include "ECS/Components.h"
#include "ECS/Prefabs.h"
#include "ECS/SceneView.h"


// ImGui Headers
#include "Editor/Editor.h"
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

// Graphics Headers
#include "Graphics/GLHeaders.h"
#include "Graphics/Renderer.h"
#include "Graphics/GUIRenderer.h"
#include "Graphics/CameraManager.h"

// Core Functionality
#include "CoreApplication.h"
#include "WindowManager.h"

// Logging and Memory
#include "Logging/Logger.h"
#include "Memory/MemoryManager.h"

// Resource Management
#include "ResourceManager/ResourceManager.h"

// Audio
#include "AudioManager/AudioManager.h"

// Time Management
#include "Time/TimeManager.h"

// Physics and Collision
#include "Physics/RigidBody.h"
#include "Physics/Colliders.h"
#include "Physics/CollisionManager.h"
#include "Physics/PhysicsManager.h"

// Serialization
#include "Data/SerializationManager.h"

// Input
#include "Input/InputSystem.h"
#include "Logic/LogicSystem.h"

#include "GUISystem.h"

// RTTR includes
#include <rttr/type.h>
#include <rttr/property.h>
#include <rttr/registration.h>

// Testing
Logger engine_logger = Logger("ENGINE");

#define TO_STR(x) #x



using namespace rttr;

RTTR_REGISTRATION
{
    REGISTERCOMPONENT(PE::EntityDescriptor);
    REGISTERCOMPONENT(PE::RigidBody);
    REGISTERCOMPONENT(PE::Collider);
    REGISTERCOMPONENT(PE::Transform);
    REGISTERCOMPONENT(PE::Graphics::Renderer);
    REGISTERCOMPONENT(PE::Graphics::Camera);
    REGISTERCOMPONENT(PE::ScriptComponent);
    REGISTERCOMPONENT(PE::GUI);
    REGISTERCOMPONENT(PE::Graphics::GUIRenderer);
    using namespace rttr;
    // test whether we need to register math lib stuff as well...
    // extra notes, will we need to include the constructor as well?
    // functionality seems fine without it... maybe needed by scripting side though
    //rttr::registration::class_<PE::vec2>("vec2")
    //    .property("x", &PE::vec2::x);
    rttr::registration::class_<PE::EntityDescriptor>(PE::EntityManager::GetInstance().GetComponentID<PE::EntityDescriptor>().to_string().c_str())
        .property("Name", &PE::EntityDescriptor::name)
        .property("Parent", &PE::EntityDescriptor::parent);

    rttr::registration::class_<PE::Transform>(PE::EntityManager::GetInstance().GetComponentID<PE::Transform>().to_string().c_str())
        .property("Position", &PE::Transform::position)
        .property("Relative Position", &PE::Transform::relPosition)
        .property("Orientation", &PE::Transform::orientation)
        .property("Width", &PE::Transform::width)
        .property("Height", &PE::Transform::height)
        .method("GetMtx3x3", &PE::Transform::GetTransformMatrix3x3);

    rttr::registration::class_<PE::RigidBody>(PE::EntityManager::GetInstance().GetComponentID<PE::RigidBody>().to_string().c_str())
        .property("Previous Position", &PE::RigidBody::prevPosition)
        .property("Velocity", &PE::RigidBody::velocity)
        .property("Rotation Velocity", &PE::RigidBody::rotationVelocity)
        .property("Force", &PE::RigidBody::force);
    
    // objects collided shouldnt be needed right? @yeni
    rttr::registration::class_<PE::Collider>(PE::EntityManager::GetInstance().GetComponentID<PE::Collider>().to_string().c_str())
        .property("Collider Type", &PE::Collider::colliderVariant)
        .property("Is Trigger", &PE::Collider::isTrigger);

    // what do i need to register here?? @krystal
    rttr::registration::class_<PE::Graphics::Renderer>(PE::EntityManager::GetInstance().GetComponentID<PE::Graphics::Renderer>().to_string().c_str())
        .method("GetColor", &PE::Graphics::Renderer::GetColor)
        .method("SetColor", &PE::Graphics::Renderer::SetColor)
        .method("GetEnabled", &PE::Graphics::Renderer::GetEnabled)
        .method("SetEnabled", &PE::Graphics::Renderer::SetEnabled)
        .method("GetTextureKey", &PE::Graphics::Renderer::GetTextureKey)
        .method("SetTextureKey", &PE::Graphics::Renderer::SetTextureKey);

    rttr::registration::class_<PE::Graphics::Camera>(PE::EntityManager::GetInstance().GetComponentID<PE::Graphics::Camera>().to_string().c_str())
        .method("GetWorldToViewMatrix", &PE::Graphics::Camera::GetWorldToViewMatrix)
        .method("GetViewToNdcMatrix", &PE::Graphics::Camera::GetViewToNdcMatrix)
        .method("GetWorldToNdcMatrix", &PE::Graphics::Camera::GetWorldToNdcMatrix)
        .method("UpdateCamera", &PE::Graphics::Camera::UpdateCamera)
        .method("GetHasChanged", &PE::Graphics::Camera::GetHasChanged)
        .method("GetUpVector", &PE::Graphics::Camera::GetUpVector)
        .method("GetRightVector", &PE::Graphics::Camera::GetRightVector)
        .method("GetAspectRatio", &PE::Graphics::Camera::GetAspectRatio)
        .method("GetMagnification", &PE::Graphics::Camera::GetMagnification)
        .method("GetViewportWidth", &PE::Graphics::Camera::GetViewportWidth)
        .method("GetViewportHeight", &PE::Graphics::Camera::GetViewportHeight)
        .method("SetViewDimensions", &PE::Graphics::Camera::SetViewDimensions)
        .method("SetMagnification", &PE::Graphics::Camera::SetMagnification)
        .method("AdjustMagnification", &PE::Graphics::Camera::AdjustMagnification);

    // is that all i need to register? @jarran
    rttr::registration::class_<PE::ScriptComponent>(PE::EntityManager::GetInstance().GetComponentID<PE::ScriptComponent>().to_string().c_str());

}

PE::CoreApplication::CoreApplication()
{
    InitializeVariables();
    RegisterComponents();

    // Load Configuration
    std::ifstream configFile("config.json");
    nlohmann::json configJson;
    configFile >> configJson;
    int width = configJson["window"]["width"];
    int height = configJson["window"]["height"];
    float windowWidth{ static_cast<float>(width) }, windowHeight{ static_cast<float>(height) };
    // Initialize Window
    m_window = m_windowManager.InitWindow(width, height, "Purring_Engine");
    TimeManager::GetInstance().m_frameRateController.SetTargetFPS(60);
    
    InitializeLogger();
    InitializeAudio();
    InitializeMemoryManager();
    InitializeSystems();


    // Load Textures and Animations
    std::string catTextureName{ "../Assets/Textures/Cat_Grey_128px.png" }, cat2TextureName{ "../Assets/Textures/Cat_Grey_Blink_128px.png" }, buttonTextureName{ "../Assets/Textures/Button_White_128px.png" };
    ResourceManager::GetInstance().LoadTextureFromFile(catTextureName, "../Assets/Textures/Cat_Grey_128px.png");
    ResourceManager::GetInstance().LoadTextureFromFile(cat2TextureName, "../Assets/Textures/Cat_Grey_Blink_128px.png");
    ResourceManager::GetInstance().LoadTextureFromFile("../Assets/Textures/bg.png", "../Assets/Textures/bg.png");

    ResourceManager::GetInstance().LoadTextureFromFile(buttonTextureName, "../Assets/Textures/Button_White_128px.png");

    

    // Animation textures
    // Animation 1
    ResourceManager::GetInstance().LoadTextureFromFile("catAnim1", "../Assets/Textures/CatSprite/Cat_Grey_128px1.png");
    ResourceManager::GetInstance().LoadTextureFromFile("catAnim2", "../Assets/Textures/CatSprite/Cat_Grey_128px2.png");
    ResourceManager::GetInstance().LoadTextureFromFile("catAnim3", "../Assets/Textures/CatSprite/Cat_Grey_128px3.png");
    ResourceManager::GetInstance().LoadTextureFromFile("catAnim4", "../Assets/Textures/CatSprite/Cat_Grey_128px4.png");
    ResourceManager::GetInstance().LoadTextureFromFile("catAnim5", "../Assets/Textures/CatSprite/Cat_Grey_128px5.png");

    // Animation 2
    ResourceManager::GetInstance().LoadTextureFromFile("cat2Anim1", "../Assets/Textures/CatSprite2/Cat_Grey_128px_Walk_2.png");
    ResourceManager::GetInstance().LoadTextureFromFile("cat2Anim2", "../Assets/Textures/CatSprite2/Cat_Grey_128px_Walk_3.png");

    SerializationManager serializationManager;
    //create background from file
    EntityManager::GetInstance().Get<EntityDescriptor>(serializationManager.LoadFromFile("../Assets/Prefabs/Background_Prefab.json")).name = "Background";
    
    // Creates an entity from file that is attached to the Character Controller
    EntityID id = serializationManager.LoadFromFile("../Assets/Prefabs/Player_Prefab.json");
    EntityManager::GetInstance().Get<EntityDescriptor>(id).name = "Player";

    

    // Create button objects
    for (int i{}; i < 2; ++i) 
    {
        EntityID buttonId = EntityFactory::GetInstance().CreateFromPrefab("ButtonObject");
        EntityManager::GetInstance().Get<Graphics::GUIRenderer>(buttonId).SetTextureKey(buttonTextureName);
        EntityManager::GetInstance().Get<Graphics::GUIRenderer>(buttonId).SetColor();
        EntityManager::GetInstance().Get<Transform>(buttonId).position.x = -125.f + 250.f * i;
        EntityManager::GetInstance().Get<Transform>(buttonId).position.y = 200.f;
        EntityManager::GetInstance().Get<Transform>(buttonId).width = 250.f;
        EntityManager::GetInstance().Get<Transform>(buttonId).height = 100.f;
    }

    // Make a runtime camera that follows the player
    EntityID cameraId = EntityFactory::GetInstance().CreateFromPrefab("CameraObject");
    EntityManager::GetInstance().Get<Graphics::Camera>(cameraId).SetViewDimensions(windowWidth, windowHeight);

    //EntityManager::GetInstance().Get<Transform>(cameraId).relPosition.x = -100.f;
    //EntityManager::GetInstance().Get<Transform>(cameraId).relPosition.y = -100.f;
    EntityManager::GetInstance().Get<EntityDescriptor>(cameraId).name = "CameraObject";
    EntityManager::GetInstance().Get<EntityDescriptor>(cameraId).parent = id;


    // Make a second runtime camera to test switching
    cameraId = EntityFactory::GetInstance().CreateFromPrefab("CameraObject");

    EntityManager::GetInstance().Get<Transform>(cameraId).relPosition.x = 100.f;
    EntityManager::GetInstance().Get<Transform>(cameraId).relPosition.y = 100.f;
    EntityManager::GetInstance().Get<EntityDescriptor>(cameraId).name = "CameraObject2";
    //EntityID child = EntityFactory::GetInstance().CreateFromPrefab("GameObject");
    //EntityManager::GetInstance().Get<EntityDescriptor>(child).name = "Child";
    //EntityManager::GetInstance().Get<EntityDescriptor>(child).parent = id;
    EntityManager::GetInstance().Get<Graphics::Camera>(cameraId).SetViewDimensions(windowWidth, windowHeight);
}

PE::CoreApplication::~CoreApplication()
{
    // future stuff can add here
}

void PE::CoreApplication::Run()
{
    // Start engine run time
    TimeManager::GetInstance().EngineStart();

    // Main Application Loop
    // Continue until the GLFW window is flagged to close
    while (!glfwWindowShouldClose(m_window))
    {
        // Time start
        TimeManager::GetInstance().StartFrame();
        engine_logger.SetTime();
        MemoryManager::GetInstance().CheckMemoryOver();
        
        // ----- UPDATE ----- //
        
        // List of keys to check for FPS adjustment
        const int keys[] = { GLFW_KEY_1, GLFW_KEY_2, GLFW_KEY_3, GLFW_KEY_4, GLFW_KEY_5, GLFW_KEY_6, GLFW_KEY_7, GLFW_KEY_8 };

        //// Iterate through the list of keys and check if any are pressed
        for (int key : keys)
        {
            // Update target FPS if a key is pressed
            if (glfwGetKey(m_window, key) == GLFW_PRESS)
            {
                TimeManager::GetInstance().m_frameRateController.UpdateTargetFPSBasedOnKey(key);
            }
        }
        if (glfwGetKey(m_window, GLFW_KEY_L) == GLFW_PRESS)
        {
            try
            {
                std::vector testVector = { 1 };
                testVector[0] = testVector.at(1);
            }
            catch (const std::out_of_range& r_err)
            {
                engine_logger.AddLog(true, r_err.what(), __FUNCTION__);
                throw r_err;
            }
        }

        //Audio Stuff - HANS
        AudioManager::GetInstance().Update();

        // Update the window title to display FPS (every second)
        double currentTime = glfwGetTime();
        if (currentTime - m_lastFrameTime >= 1.0)
        {
            m_windowManager.UpdateTitle(m_window, TimeManager::GetInstance().m_frameRateController.GetFps());
            m_lastFrameTime = currentTime;
        }

        for (const auto& id : SceneView())
        {
            Transform& trans = EntityManager::GetInstance().Get<Transform>(id);
            if (EntityManager::GetInstance().Get<EntityDescriptor>(id).parent.has_value())
            {
                const Transform& parent = EntityManager::GetInstance().Get<Transform>(EntityManager::GetInstance().Get<EntityDescriptor>(id).parent.value());
                vec3 tmp { trans.relPosition, 1.f };
                tmp = parent.GetTransformMatrix3x3() * tmp;
                trans.position.x = tmp.x;
                trans.position.y = tmp.y;
                trans.orientation = parent.orientation + trans.relOrientation;
            }
        }

        // Update system with fixed time step
        TimeManager::GetInstance().StartAccumulator();
        while (TimeManager::GetInstance().UpdateAccumulator())
        { 
            for (SystemID systemID{}; systemID < SystemID::GRAPHICS; ++systemID)
            {
                TimeManager::GetInstance().SystemStartFrame(systemID);
                m_systemList[systemID]->UpdateSystem(TimeManager::GetInstance().GetFixedTimeStep());
                TimeManager::GetInstance().SystemEndFrame(systemID);
            }
            TimeManager::GetInstance().EndAccumulator();
        }

        // Update Graphics with variable timestep
        TimeManager::GetInstance().SystemStartFrame(SystemID::GRAPHICS);
        m_systemList[SystemID::GRAPHICS]->UpdateSystem(TimeManager::GetInstance().GetDeltaTime());
        TimeManager::GetInstance().SystemEndFrame(SystemID::GRAPHICS);

        // Flush log entries
        engine_logger.FlushLog();

        TimeManager::GetInstance().EndFrame();
        // Finalize FPS calculations for the current frame
        TimeManager::GetInstance().m_frameRateController.EndFrame();
    }

    // Cleanup for ImGui
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    // Additional Cleanup (if required)
    m_windowManager.Cleanup();
    ResourceManager::GetInstance().UnloadResources();
}

void PE::CoreApplication::Initialize()
{
    // Init all systems and iterate through each system in m_systemList and initialize it
    for (System* system : m_systemList)
    {
        system->InitializeSystem();                             // Call the InitializeSystem method for each system
    }
}

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

void PE::CoreApplication::AddSystem(System* system)
{
    // Add a system to CoreApplication append the provided system pointer to the m_systemList vector
    m_systemList.push_back(system);
}


void PE::CoreApplication::InitializeVariables()
{
    m_Running = true;
    m_lastFrameTime = 0;
}

void PE::CoreApplication::RegisterComponents()
{

    //std::cout << "TRANSFORM COMPONENT PROPERTIES: \n";
    //rttr::type cls = rttr::type::get_by_name(PE::EntityManager::GetInstance().GetComponentID<PE::Transform>().to_string());
    //for (auto& prop : cls.get_properties())
    //{
    //    std::cout << "name: " << prop.get_name() << std::endl;
    //}
    //for (auto& meth : cls.get_methods())
    //{
    //    std::cout << "name: " << meth.get_name() << std::endl;
    //}

    //std::cout << "\nTRANSFORM COMPONENT Orientation value: ";
    //Transform tmp;

    //property p = rttr::type::get_by_name(PE::EntityManager::GetInstance().GetComponentID<PE::Transform>().to_string()).get_property("orientation");
    //p.set_value(tmp, 69.f);

    //variant vp = p.get_value(tmp);
    //std::cout << vp.to_float() << std::endl;

    //std::cout << "\nRIGIDBODY COMPONENT PROPERTIES: \n";

    //cls = rttr::type::get_by_name(PE::EntityManager::GetInstance().GetComponentID<PE::RigidBody>().to_string());
    //for (auto& prop : cls.get_properties())
    //{
    //    std::cout << "name: " << prop.get_name() << std::endl;
    //}
    //for (auto& meth : cls.get_methods())
    //{
    //    std::cout << "name: " << meth.get_name() << std::endl;
    //}

    //std::cout << "\nCOLLIDER COMPONENT PROPERTIES: \n";
    //cls = rttr::type::get_by_name(PE::EntityManager::GetInstance().GetComponentID<PE::Collider>().to_string());
    //for (auto& prop : cls.get_properties())
    //{
    //    std::cout << "name: " << prop.get_name() << std::endl;
    //}
    //for (auto& meth : cls.get_methods())
    //{
    //    std::cout << "name: " << meth.get_name() << std::endl;
    //}

    //std::cout << std::endl;

    rttr::type cls = rttr::type::get_by_name(PE::EntityManager::GetInstance().GetComponentID<PE::Collider>().to_string());
    for (auto& prop : cls.get_properties())
    {
        std::cout << "name: " << prop.get_name() << std::endl;
        std::cout << "type: " << prop.get_type().get_name() << std::endl;

    }
    for (auto& meth : cls.get_methods())
    {
        std::cout << "name: " << meth.get_name() << std::endl;
    }
}



void PE::CoreApplication::InitializeLogger()
{
    engine_logger.SetFlag(Logger::EnumLoggerFlags::WRITE_TO_CONSOLE | Logger::EnumLoggerFlags::WRITE_TO_FILE | Logger::EnumLoggerFlags::DEBUG, true);
    engine_logger.SetTime();
    engine_logger.AddLog(false, "Engine initialized!", __FUNCTION__);
}


void PE::CoreApplication::InitializeAudio()
{
    AudioManager::GetInstance().Init();
    {
        engine_logger.AddLog(false, "AudioManager initialized!", __FUNCTION__);
    }
    ResourceManager::GetInstance().LoadAudioFromFile("audio_sound1", "../Assets/Audio/audioSFX_sound1.mp3");
    ResourceManager::GetInstance().LoadAudioFromFile("audio_sound2", "../Assets/Audio/audioSFX_sound2.mp3");
    ResourceManager::GetInstance().LoadAudioFromFile("audio_sound3", "../Assets/Audio/audioSFX_sound3.mp3");
    ResourceManager::GetInstance().LoadAudioFromFile("audio_backgroundMusic", "../Assets/Audio/audioSFX_backgroundMusic.mp3");
}


void PE::CoreApplication::InitializeMemoryManager()
{
    MemoryManager::GetInstance();
}

void PE::CoreApplication::InitializeSystems()
{
    // Get the window width and height to initialize the camera manager with
    int width, height;
    glfwGetWindowSize(m_window, &width, &height);

    // Add system to list & assigning memory to them

    LogicSystem* p_logicSystem = new (MemoryManager::GetInstance().AllocateMemory("Logic System", sizeof(LogicSystem)))LogicSystem{};
    Graphics::CameraManager* p_cameraManager = new (MemoryManager::GetInstance().AllocateMemory("Camera Manager", sizeof(Graphics::CameraManager)))Graphics::CameraManager{ static_cast<float>(width), static_cast<float>(height) };
    Graphics::RendererManager* p_rendererManager = new (MemoryManager::GetInstance().AllocateMemory("Renderer Manager", sizeof(Graphics::RendererManager)))Graphics::RendererManager{ m_window, *p_cameraManager };
    PhysicsManager* p_physicsManager = new (MemoryManager::GetInstance().AllocateMemory("Physics Manager", sizeof(PhysicsManager)))PhysicsManager{};
    CollisionManager* p_collisionManager = new (MemoryManager::GetInstance().AllocateMemory("Collision Manager", sizeof(CollisionManager)))CollisionManager{};
    InputSystem* p_inputSystem = new (MemoryManager::GetInstance().AllocateMemory("Input System", sizeof(InputSystem)))InputSystem{};
    GUISystem* p_guisystem = new (MemoryManager::GetInstance().AllocateMemory("GUI System", sizeof(GUISystem)))GUISystem{ m_window };
    AddSystem(p_inputSystem);
    AddSystem(p_guisystem);
    AddSystem(p_logicSystem);
    AddSystem(p_physicsManager);
    AddSystem(p_collisionManager);
    AddSystem(p_cameraManager);
    AddSystem(p_rendererManager);
}