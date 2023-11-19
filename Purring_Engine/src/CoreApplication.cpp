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

#ifndef GAMERELEASE
// ImGui Headers
#include "Editor/Editor.h"
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#endif // !GAMERELEASE

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

#include "GUISystem.h"

// RTTR includes
#include <rttr/type.h>
#include <rttr/property.h>
#include <rttr/registration.h>

// Logic
#include "Logic/LogicSystem.h"
#include "Logic/PlayerControllerScript.h"
#include "Logic/EnemyTestScript.h"
#include "Logic/testScript.h"
#include "Logic/testScript2.h"
#include "Logic/FollowScript.h"
#include "Logic/CameraManagerScript.h"
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
    REGISTERCOMPONENT(PE::AnimationComponent);
    REGISTERCOMPONENT(PE::TextComponent);
   
    using namespace rttr;
    // test whether we need to register math lib stuff as well...
    // extra notes, will we need to include the constructor as well?
    // functionality seems fine without it... maybe needed by scripting side though
    //rttr::registration::class_<PE::vec2>("vec2")
    //    .property("x", &PE::vec2::x);
    rttr::registration::class_<PE::EntityDescriptor>(PE::EntityManager::GetInstance().GetComponentID<PE::EntityDescriptor>().to_string().c_str())
        .property("Name", &PE::EntityDescriptor::name)
        .property_readonly("Parent", &PE::EntityDescriptor::parent);

    rttr::registration::class_<PE::Transform>(PE::EntityManager::GetInstance().GetComponentID<PE::Transform>().to_string().c_str())
        .property("Position", &PE::Transform::position)
        .property("Relative Position", &PE::Transform::relPosition)
        .property("Relative Orientation", &PE::Transform::relOrientation)
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
    rttr::registration::class_<PE::ScriptComponent>(PE::EntityManager::GetInstance().GetComponentID<PE::ScriptComponent>().to_string().c_str())
        .property("ScriptKeys", &PE::ScriptComponent::m_scriptKeys);

    rttr::registration::class_<PE::PlayerControllerScriptData>("PlayerControllerScript")
        .property("PlayerState", &PE::PlayerControllerScriptData::currentPlayerState)
        .property("HP", &PE::PlayerControllerScriptData::HP)
        .property("speed", &PE::PlayerControllerScriptData::speed);

    rttr::registration::class_<PE::EnemyTestScriptData>("EnemyTestScript")
        .property("PlayerID", &PE::EnemyTestScriptData::playerID)
        .property("speed", &PE::EnemyTestScriptData::speed)
        .property("idleTimer", &PE::EnemyTestScriptData::idleTimer)
        .property("alertTimer", &PE::EnemyTestScriptData::alertTimer)
        .property("timerBuffer", &PE::EnemyTestScriptData::timerBuffer)
        .property("patrolTimer", &PE::EnemyTestScriptData::patrolBuffer)
        .property("distanceFromPlayer", &PE::EnemyTestScriptData::distanceFromPlayer)
        .property("TargetRange", &PE::EnemyTestScriptData::TargetRange)
        .property("bounce", &PE::EnemyTestScriptData::bounce);


    rttr::registration::class_<PE::TestScriptData>("testScript")
        .property("m_rotationSpeed", &PE::TestScriptData::m_rotationSpeed);

    rttr::registration::class_<PE::AnimationComponent>(PE::EntityManager::GetInstance().GetComponentID<PE::AnimationComponent>().to_string().c_str())
        .method("GetCurrentAnimation", &PE::AnimationComponent::GetAnimationID)
        .method("SetCurrentAntimation", &PE::AnimationComponent::SetAnimationID);

    rttr::registration::class_<PE::FollowScriptData>("FollowScript")
        .property("Size", &PE::FollowScriptData::Size)
        .property("Distance", &PE::FollowScriptData::Distance)
        .property("Speed", &PE::FollowScriptData::Speed)
        .property("NumberOfFollower", &PE::FollowScriptData::NumberOfFollower)
        .property("FollowingObject", &PE::FollowScriptData::FollowingObject)
        .property("rotation", &PE::FollowScriptData::Rotation)
        .property("CurrentPosition", &PE::FollowScriptData::CurrentPosition)
        .property("NextPosition", &PE::FollowScriptData::NextPosition);

    rttr::registration::class_<PE::CameraManagerScriptData>("CameraManagerScript")
        .property("NumberOfCamera", &PE::CameraManagerScriptData::NumberOfCamera)
        .property("CameraIDs", &PE::CameraManagerScriptData::CameraIDs);


    rttr::registration::class_<PE::TextComponent>(PE::EntityManager::GetInstance().GetComponentID<PE::TextComponent>().to_string().c_str())
        .property_readonly("Font", &PE::TextComponent::GetFontKey)
        .property_readonly("Size", &PE::TextComponent::GetSize)
        .property_readonly("Text", &PE::TextComponent::GetText)
        .property_readonly("Color", &PE::TextComponent::GetColor)
        .method("Color", &PE::TextComponent::SetColor)
        .method("Text", &PE::TextComponent::SetText)
        .method("Size", &PE::TextComponent::SetSize)
        .method("Font", &PE::TextComponent::SetFont);
}

PE::CoreApplication::CoreApplication()
{
    InitializeVariables();

    // Load Configuration
    std::ifstream configFile("../Assets/Settings/config.json");
    nlohmann::json configJson;
    configFile >> configJson;
    int width = configJson["window"]["width"];
    int height = configJson["window"]["height"];
    
    // Initialize Window
    m_window = m_windowManager.InitWindow(width, height, "Purring_Engine");
    TimeManager::GetInstance().m_frameRateController.SetTargetFPS(60);
    
    InitializeLogger();
    InitializeAudio();
    InitializeMemoryManager();
    InitializeSystems();


    // Load Textures and Animations
    std::string catTextureName{ "../Assets/Textures/Cat_Grey_128px.png" }, cat2TextureName{ "../Assets/Textures/Cat_Grey_Blink_128px.png" }, buttonTextureName{ "../Assets/Textures/Button_White_128px.png" };
    std::string ratTextureName{ "../Assets/Textures/Rat_Brawler_128px.png" };
    ResourceManager::GetInstance().LoadTextureFromFile(catTextureName, "../Assets/Textures/Cat_Grey_128px.png");
    ResourceManager::GetInstance().LoadTextureFromFile(cat2TextureName, "../Assets/Textures/Cat_Grey_Blink_128px.png");
    ResourceManager::GetInstance().LoadTextureFromFile(ratTextureName, "../Assets/Textures/Rat_Gutter_128px.png");
    ResourceManager::GetInstance().LoadTextureFromFile("../Assets/Textures/bg.png", "../Assets/Textures/bg.png");

    ResourceManager::GetInstance().LoadTextureFromFile(buttonTextureName, "../Assets/Textures/Button_White_128px.png");

    // Load Fonts
    std::string fontHeader{ "../Assets/Fonts/Kalam/Kalam-Regular.ttf" }, fontBody{ "../Assets/Fonts/Caveat/static/Caveat-Regular.ttf" };
    ResourceManager::GetInstance().LoadFontFromFile(fontHeader, "../Assets/fonts/Kalam/Kalam-Regular.ttf");
    ResourceManager::GetInstance().LoadFontFromFile(fontBody, "../Assets/Fonts/Caveat/static/Caveat-Regular.ttf");    

    // Animation textures
    std::string catWalkSpriteSheet{ "../Assets/Textures/Animations/Individual Rows/Cat_Grey_Walk.png" };
    std::string catAttackSpriteSheet{ "../Assets/Textures/Animations/Individual Rows/Cat_Grey_Attack.png" };
    std::string ratAttackSpriteSheet{ "../Assets/Textures/Animations/Individual Rows/Rat_Gutter_Attack.png" };
    std::string ratDeathSpriteSheet{ "../Assets/Textures/Animations/Individual Rows/Rat_Gutter_Death.png" };

    // Spritesheet 1
    ResourceManager::GetInstance().LoadTextureFromFile(catWalkSpriteSheet, "../Assets/Textures/Animations/Individual Rows/Cat_Grey_Walk.png");

    // Spritesheet 2
    ResourceManager::GetInstance().LoadTextureFromFile(catAttackSpriteSheet, "../Assets/Textures/Animations/Individual Rows/Cat_Grey_Attack.png");

    // Spritesheet 3
    ResourceManager::GetInstance().LoadTextureFromFile(ratAttackSpriteSheet, "../Assets/Textures/Animations/Individual Rows/Rat_Gutter_Attack.png");

    // Spritesheet 4
    ResourceManager::GetInstance().LoadTextureFromFile(ratDeathSpriteSheet, "../Assets/Textures/Animations/Individual Rows/Rat_Gutter_Death.png");

    SerializationManager serializationManager;
    //create background from file

    EntityID uiCameraId{ serializationManager.LoadFromFile("../Assets/Prefabs/Camera_Prefab.json") };
    Graphics::CameraManager::SetUiCamera(uiCameraId);
    EntityManager::GetInstance().Get<EntityDescriptor>(uiCameraId).name = "UI Camera";

    // EntityID bgId{ serializationManager.LoadFromFile("../Assets/Prefabs/Background_Prefab.json") };
    // EntityManager::GetInstance().Get<EntityDescriptor>(bgId).name = "Background";
    // EntityManager::GetInstance().Get<Transform>(bgId).width = 1920.f;
    // EntityManager::GetInstance().Get<Transform>(bgId).height = 1080.f;
    
    

    // Create button objects
    //for (int i{}; i < 2; ++i) 
    //{
    //    EntityID buttonId = EntityFactory::GetInstance().CreateFromPrefab("ButtonObject");
    //    EntityManager::GetInstance().Get<Graphics::GUIRenderer>(buttonId).SetTextureKey(buttonTextureName);
    //    EntityManager::GetInstance().Get<Graphics::GUIRenderer>(buttonId).SetColor();
    //    EntityManager::GetInstance().Get<Transform>(buttonId).position.x = -125.f + 250.f * i;
    //    EntityManager::GetInstance().Get<Transform>(buttonId).position.y = 200.f;
    //    EntityManager::GetInstance().Get<Transform>(buttonId).width = 250.f;
    //    EntityManager::GetInstance().Get<Transform>(buttonId).height = 100.f;
    //}

    // Make a runtime camera that follows the player
    //EntityID cameraId = EntityFactory::GetInstance().CreateFromPrefab("CameraObject");
    //EntityManager::GetInstance().Get<Graphics::Camera>(cameraId).SetViewDimensions(windowWidth, windowHeight);

    //EntityManager::GetInstance().Get<Transform>(cameraId).relPosition.x = -100.f;
    //EntityManager::GetInstance().Get<Transform>(cameraId).relPosition.y = -100.f;
    //EntityManager::GetInstance().Get<EntityDescriptor>(cameraId).name = "CameraObject";
    //EntityManager::GetInstance().Get<EntityDescriptor>(cameraId).parent = id;


    // Make a second runtime camera to test switching
   // cameraId = EntityFactory::GetInstance().CreateFromPrefab("CameraObject");


    //EntityManager::GetInstance().Get<Transform>(cameraId).position.x = 100.f;
   // EntityManager::GetInstance().Get<Transform>(cameraId).position.y = 100.f;
   // EntityManager::GetInstance().Get<EntityDescriptor>(cameraId).name = "CameraObject2";
    //EntityID child = EntityFactory::GetInstance().CreateFromPrefab("GameObject");
    //EntityManager::GetInstance().Get<EntityDescriptor>(child).name = "Child";
    //EntityManager::GetInstance().Get<EntityDescriptor>(child).parent = id;

    // Create animations here for now
    std::string playerWalkAnimation, playerAttackAnimation, ratAttackAnimation, ratDeathAnimation;
    playerWalkAnimation = AnimationManager::CreateAnimation("playerWalk", catWalkSpriteSheet);
    playerAttackAnimation = AnimationManager::CreateAnimation("playerAttack", catAttackSpriteSheet);
    ratAttackAnimation = AnimationManager::CreateAnimation("ratAttack", ratAttackSpriteSheet);
    ratDeathAnimation = AnimationManager::CreateAnimation("ratDeath", ratDeathSpriteSheet);

    // animation 1
    AnimationManager::AddFrameToAnimation(playerWalkAnimation, { 0.f, 0.f }, { 1.f / 6.f, 1.f }, 1.f / 6.f);
    AnimationManager::AddFrameToAnimation(playerWalkAnimation, { 1.f / 6.f, 0.f }, { 2.f / 6.f, 1.f }, 1.f / 6.f);
    AnimationManager::AddFrameToAnimation(playerWalkAnimation, { 2.f / 6.f, 0.f }, { 3.f / 6.f, 1.f }, 1.f / 6.f);
    AnimationManager::AddFrameToAnimation(playerWalkAnimation, { 3.f / 6.f, 0.f }, { 4.f / 6.f, 1.f }, 1.f / 6.f);
    AnimationManager::AddFrameToAnimation(playerWalkAnimation, { 4.f / 6.f, 0.f }, { 5.f / 6.f, 1.f }, 1.f / 6.f);
    AnimationManager::AddFrameToAnimation(playerWalkAnimation, { 5.f / 6.f, 0.f }, { 1.f, 1.f }, 1.f / 6.f);

    // animation 2
    AnimationManager::AddFrameToAnimation(playerAttackAnimation, { 0.f, 0.f }, { 1.f / 6.f, 1.f }, 1.f / 6.f);
    AnimationManager::AddFrameToAnimation(playerAttackAnimation, { 1.f / 6.f, 0.f }, { 2.f / 6.f, 1.f }, 1.f / 6.f);
    AnimationManager::AddFrameToAnimation(playerAttackAnimation, { 2.f / 6.f, 0.f }, { 3.f / 6.f, 1.f }, 1.f / 6.f);
    AnimationManager::AddFrameToAnimation(playerAttackAnimation, { 3.f / 6.f, 0.f }, { 4.f / 6.f, 1.f }, 1.f / 6.f);
    AnimationManager::AddFrameToAnimation(playerAttackAnimation, { 4.f / 6.f, 0.f }, { 5.f / 6.f, 1.f }, 1.f / 6.f);
    AnimationManager::AddFrameToAnimation(playerAttackAnimation, { 5.f / 6.f, 0.f }, { 1.f, 1.f }, 1.f / 6.f);

    // animation 3
    AnimationManager::AddFrameToAnimation(ratAttackAnimation, { 0.f, 0.f }, { 1.f / 6.f, 1.f }, 1.f / 6.f);
    AnimationManager::AddFrameToAnimation(ratAttackAnimation, { 1.f / 6.f, 0.f }, { 2.f / 6.f, 1.f }, 1.f / 6.f);
    AnimationManager::AddFrameToAnimation(ratAttackAnimation, { 2.f / 6.f, 0.f }, { 3.f / 6.f, 1.f }, 1.f / 6.f);
    AnimationManager::AddFrameToAnimation(ratAttackAnimation, { 3.f / 6.f, 0.f }, { 4.f / 6.f, 1.f }, 1.f / 6.f);
    AnimationManager::AddFrameToAnimation(ratAttackAnimation, { 4.f / 6.f, 0.f }, { 5.f / 6.f, 1.f }, 1.f / 6.f);
    AnimationManager::AddFrameToAnimation(ratAttackAnimation, { 5.f / 6.f, 0.f }, { 1.f, 1.f }, 1.f / 6.f);

    // animation 4
    AnimationManager::AddFrameToAnimation(ratDeathAnimation, { 0.f, 0.f }, { 1.f / 6.f, 1.f }, 1.f / 6.f);
    AnimationManager::AddFrameToAnimation(ratDeathAnimation, { 1.f / 6.f, 0.f }, { 2.f / 6.f, 1.f }, 1.f / 6.f);
    AnimationManager::AddFrameToAnimation(ratDeathAnimation, { 2.f / 6.f, 0.f }, { 3.f / 6.f, 1.f }, 1.f / 6.f);
    AnimationManager::AddFrameToAnimation(ratDeathAnimation, { 3.f / 6.f, 0.f }, { 4.f / 6.f, 1.f }, 1.f / 6.f);
    AnimationManager::AddFrameToAnimation(ratDeathAnimation, { 4.f / 6.f, 0.f }, { 5.f / 6.f, 1.f }, 1.f / 6.f);
    AnimationManager::AddFrameToAnimation(ratDeathAnimation, { 5.f / 6.f, 0.f }, { 1.f, 1.f }, 1.f / 6.f);

}

PE::CoreApplication::~CoreApplication()
{
    // future stuff can add here
}

void PE::CoreApplication::Run()
{
    // Start engine run time
    TimeManager::GetInstance().EngineStart();

    SerializationManager serializationManager;

    // Loads Default Scene
    serializationManager.LoadAllEntitiesFromFile("../Assets/Scenes/DefaultScene.json");

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

        //Audio Stuff - HANS
        AudioManager::GetInstance().Update();

        // Update the window title to display FPS (every second)
        double currentTime = glfwGetTime();
        if (currentTime - m_lastFrameTime >= 1.0)
        {
            m_windowManager.UpdateTitle(m_window, TimeManager::GetInstance().m_frameRateController.GetFps());
            m_lastFrameTime = currentTime;
        }

        for (const auto& id : SceneView<Transform>())
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
#ifndef GAMERELEASE
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
#endif // !GAMERELEASE

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
    AnimationManager* p_animationManager = new (MemoryManager::GetInstance().AllocateMemory("Animation System", sizeof(AnimationManager)))AnimationManager{};

    AddSystem(p_inputSystem);
    AddSystem(p_guisystem);
    AddSystem(p_logicSystem);
    AddSystem(p_physicsManager);
    AddSystem(p_collisionManager);
    AddSystem(p_animationManager);
    AddSystem(p_cameraManager);
    AddSystem(p_rendererManager);
}