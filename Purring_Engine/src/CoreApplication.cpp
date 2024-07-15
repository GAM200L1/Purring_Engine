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
#include "Graphics/Cursor.h"

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
#include "AudioManager/AudioComponent.h"

// Time Management
#include "Time/TimeManager.h"

// Physics and Collision
#include "Physics/RigidBody.h"
#include "Physics/Colliders.h"
#include "Physics/CollisionManager.h"
#include "Physics/PhysicsManager.h"

// Visual Effects
#include "VisualEffects/VisualEffectsManager.h"
#include "VisualEffects/ParticleSystem.h"

// Serialization
#include "Data/SerializationManager.h"

// Input
#include "Input/InputSystem.h"

#include "GUISystem.h"
#include "GUI/Canvas.h"

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
#include "Logic/CatScript.h"
#include "Logic/GameStateController.h"
#include "GameStateManager.h"
#include "Logic/CatScript.h"
#include "Logic/RatScript.h"
#include "Logic/GameStateController_v2_0.h"
#include "Logic/UI/HealthBarScript_v2_0.h"
#include "Logic/Rat/RatScript_v2_0.h"
#include "Logic/DeploymentScript.h"
#include "Logic/MainMenuController.h"
#include "Logic/IntroCutsceneController.h"
#include "Logic/EndingCutsceneController.h"

#include "Logic/Boss/BossRatScript.h"
#include "Logic/ObjectAttachScript.h"
#include "Logic/Settings.h"

#include "Logic/Cat/CatController_v2_0.h"
#include "Logic/Cat/CatScript_v2_0.h"

#include "Logic/TutorialController.h"

#include "Logic/CameraShakeScript.h"

// Scene Manager
#include "SceneManager/SceneManager.h"

#include "Hierarchy/HierarchyManager.h"

#include "Layers/LayerManager.h"

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
    REGISTERCOMPONENT(PE::GUIButton);
    REGISTERCOMPONENT(PE::GUISlider);
    REGISTERCOMPONENT(PE::Graphics::GUIRenderer);
    REGISTERCOMPONENT(PE::AnimationComponent);
    REGISTERCOMPONENT(PE::TextComponent);
    REGISTERCOMPONENT(PE::AudioComponent);
    REGISTERCOMPONENT(PE::Canvas);
    REGISTERCOMPONENT(PE::ParticleEmitter);
   
    using namespace rttr;
    // test whether we need to register math lib stuff as well...
    // extra notes, will we need to include the constructor as well?
    // functionality seems fine without it... maybe needed by scripting side though
    //rttr::registration::class_<PE::vec2>("vec2")
    //    .property("x", &PE::vec2::x);
    rttr::registration::class_<PE::EntityDescriptor>(PE::EntityManager::GetInstance().GetComponentID<PE::EntityDescriptor>().to_string().c_str())
        .property("Name", &PE::EntityDescriptor::name)
        .property_readonly("Entity ID", &PE::EntityDescriptor::oldID)
        .property_readonly("Scene ID", &PE::EntityDescriptor::sceneID)
        .property("Active", &PE::EntityDescriptor::isActive)
        .property("Render Layer", &PE::EntityDescriptor::layer)
        .property("Interaction Layer", &PE::EntityDescriptor::interactionLayer)
        .property_readonly("Parent", &PE::EntityDescriptor::parent)
        .property_readonly("Prefab Type", &PE::EntityDescriptor::prefabType);

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
        .method("AdjustMagnification", &PE::Graphics::Camera::AdjustMagnification)
        .method("SetBackgroundColor", &PE::Graphics::Camera::SetBackgroundColor)
        .method("GetBackgroundColor", &PE::Graphics::Camera::GetBackgroundColor);

    // is that all i need to register? @jarran
    rttr::registration::class_<PE::ScriptComponent>(PE::EntityManager::GetInstance().GetComponentID<PE::ScriptComponent>().to_string().c_str())
        .property("ScriptKeys", &PE::ScriptComponent::m_scriptKeys);

    rttr::registration::class_<PE::PlayerControllerScriptData>("PlayerControllerScript")
        .property("PlayerState", &PE::PlayerControllerScriptData::currentPlayerState)
        .property("HP", &PE::PlayerControllerScriptData::HP)
        .property("speed", &PE::PlayerControllerScriptData::speed);

    rttr::registration::class_<PE::TutorialControllerData>("TutorialController")
        .property("TutorialPanel1", &PE::TutorialControllerData::TutorialPanel1)
        .property("TutorialPanel2", &PE::TutorialControllerData::TutorialPanel2)
        .property("TutorialPanel3", &PE::TutorialControllerData::TutorialPanel3);


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

    rttr::registration::class_<PE::GameStateController_v2_0Data>("GameStateController_v2_0")
        .property("GameStateManagerActive", &PE::GameStateController_v2_0Data::GameStateManagerActive)
        .property("PauseBackGroundCanvas", &PE::GameStateController_v2_0Data::PauseBackGroundCanvas)
        .property("PauseMenuCanvas", &PE::GameStateController_v2_0Data::PauseMenuCanvas)
        .property("AreYouSureCanvas", &PE::GameStateController_v2_0Data::AreYouSureCanvas)
        .property("AreYouSureRestartCanvas", &PE::GameStateController_v2_0Data::AreYouSureRestartCanvas)
        .property("LoseCanvas", &PE::GameStateController_v2_0Data::LoseCanvas)
        .property("WinCanvas", &PE::GameStateController_v2_0Data::WinCanvas)
        .property("HUDCanvas", &PE::GameStateController_v2_0Data::HUDCanvas)
        .property("ExecuteCanvas", &PE::GameStateController_v2_0Data::ExecuteCanvas)
        .property("TurnCounterCanvas", &PE::GameStateController_v2_0Data::TurnCounterCanvas)
        .property("HowToPlayCanvas", &PE::GameStateController_v2_0Data::HowToPlayCanvas)
        .property("HowToPlayPageOne", &PE::GameStateController_v2_0Data::HowToPlayPageOne)
        .property("HowToPlayPageTwo", &PE::GameStateController_v2_0Data::HowToPlayPageTwo)
        .property("CatPortrait", &PE::GameStateController_v2_0Data::CatPortrait)
        .property("RatPortrait", &PE::GameStateController_v2_0Data::RatPortrait)
        .property("RatKingPortrait", &PE::GameStateController_v2_0Data::RatKingPortrait)
        .property("Portrait", &PE::GameStateController_v2_0Data::Portrait)
        .property("Background", &PE::GameStateController_v2_0Data::Background)
        .property("TransitionPanel", &PE::GameStateController_v2_0Data::TransitionPanel)
        .property("Journal", &PE::GameStateController_v2_0Data::Journal)
        .property("RatKingJournal", &PE::GameStateController_v2_0Data::RatKingJournal)
        .property("JournalButton", &PE::GameStateController_v2_0Data::JournalButton)
        .property("PhaseBanner", &PE::GameStateController_v2_0Data::PhaseBanner)
        .property("SettingsMenu", &PE::GameStateController_v2_0Data::SettingsMenu)
        .property("JournalIcon", &PE::GameStateController_v2_0Data::JournalIcon)
        .property("WindParticles", &PE::GameStateController_v2_0Data::WindParticles)
        .property("SnowParticles", &PE::GameStateController_v2_0Data::SnowParticles)
        .property("GodModeText", &PE::GameStateController_v2_0Data::GodModeText)
        .property("clicklisttest", &PE::GameStateController_v2_0Data::clicklisttest);

    //rttr::registration::class_<PE::GameStateController>("GameStateController")
    //    .property("GameStateManagerActive", &PE::GameStateControllerData::GameStateManagerActive)
    //    .property("SplashScreen", &PE::GameStateControllerData::SplashScreen)
    //    .property("executingStatement", &PE::GameStateControllerData::executingStatement)
    //    .property("mapOverlay", &PE::GameStateControllerData::mapOverlay)
    //    .property("pawOverlay", &PE::GameStateControllerData::pawOverlay)
    //    .property("foliageOverlay", &PE::GameStateControllerData::foliageOverlay)
    //    .property("energyHeader", &PE::GameStateControllerData::energyHeader)
    //    .property("currentEnergyText", &PE::GameStateControllerData::currentEnergyText)
    //    .property("slashText", &PE::GameStateControllerData::slashText)
    //    .property("maxEnergyText", &PE::GameStateControllerData::maxEnergyText)
    //    .property("energyBackground", &PE::GameStateControllerData::energyBackground)
    //    .property("turnNumberText", &PE::GameStateControllerData::turnNumberText)
    //    .property("planAttackText", &PE::GameStateControllerData::planAttackText)
    //    .property("planMovementText", &PE::GameStateControllerData::planMovementText)
    //    .property("turnBackground", &PE::GameStateControllerData::turnBackground)
    //    .property("endTurnButton", &PE::GameStateControllerData::endTurnButton)
    //    .property("endMovementText", &PE::GameStateControllerData::endMovementText)
    //    .property("endTurnText", &PE::GameStateControllerData::endTurnText);

    rttr::registration::class_<PE::DeploymentScriptData>("DeploymentScript")
        .property("FollowingTextureObject", &PE::DeploymentScriptData::FollowingTextureObject)
        .property("NoGoArea", &PE::DeploymentScriptData::NoGoArea)
        .property("DeploymentArea", &PE::DeploymentScriptData::DeploymentArea);   
    
    rttr::registration::class_<PE::MainMenuControllerData>("MainMenuController")
        .property("AreYouSureCanvas", &PE::MainMenuControllerData::AreYouSureCanvas)
        .property("MainMenuCanvas", &PE::MainMenuControllerData::MainMenuCanvas)
        .property("SplashScreen", &PE::MainMenuControllerData::SplashScreen)
        .property("HowToPlayCanvas", &PE::MainMenuControllerData::HowToPlayCanvas)
        .property("HowToPlayPageOne", &PE::MainMenuControllerData::HowToPlayPageOne)
        .property("HowToPlayPageTwo", &PE::MainMenuControllerData::HowToPlayPageTwo)
        .property("CreditsCanvas", &PE::MainMenuControllerData::CreditsCanvas)
        .property("CreditsPageOne", &PE::MainMenuControllerData::CreditsPageOne)
        .property("CreditsPageTwo", &PE::MainMenuControllerData::CreditsPageTwo)
        .property("TransitionPanel", &PE::MainMenuControllerData::TransitionPanel)
        .property("SettingsMenu", &PE::MainMenuControllerData::SettingsMenu);

    rttr::registration::class_<PE::TestScriptData>("testScript")
        .property("m_rotationSpeed", &PE::TestScriptData::m_rotationSpeed);    

    rttr::registration::class_<PE::SettingsScriptData>("SettingsScript")
        .property("BGMSlider", &PE::SettingsScriptData::BGMSlider)
        .property("SFXSlider", &PE::SettingsScriptData::SFXSlider)
        .property("MasterSlider", &PE::SettingsScriptData::MasterSlider)
        .property("FullScreenButton", &PE::SettingsScriptData::FullScreenButton)
        .property("WindowedButton", &PE::SettingsScriptData::WindowedButton);
    
    rttr::registration::class_<PE::IntroCutsceneControllerData>("IntroCutsceneController")
        .property("CutsceneObject", &PE::IntroCutsceneControllerData::CutsceneObject)
        .property("FinalScene", &PE::IntroCutsceneControllerData::FinalScene)
        .property("Text", &PE::IntroCutsceneControllerData::Text)
        .property("SkipButton", &PE::IntroCutsceneControllerData::SkipButton)
        .property("ContinueButton", &PE::IntroCutsceneControllerData::ContinueButton)
        .property("TransitionScreen", &PE::IntroCutsceneControllerData::TransitionScreen);

    rttr::registration::class_<PE::EndingCutsceneControllerData>("EndingCutsceneController")
        .property("CutsceneObject", &PE::EndingCutsceneControllerData::CutsceneObject)
        .property("FinalScene", &PE::EndingCutsceneControllerData::FinalScene)
        .property("Text", &PE::EndingCutsceneControllerData::Text)
        .property("AreYouSureCanvas", &PE::EndingCutsceneControllerData::AreYouSureCanvas)
        .property("BackgroundCanvas", &PE::EndingCutsceneControllerData::BackgroundCanvas)
        .property("WinCanvas", &PE::EndingCutsceneControllerData::WinCanvas)
        .property("SkipButton", &PE::EndingCutsceneControllerData::SkipButton)
        .property("ContinueButton", &PE::EndingCutsceneControllerData::ContinueButton)
        .property("TransitionScreen", &PE::EndingCutsceneControllerData::TransitionScreen)
        .property("DeathCounter", &PE::EndingCutsceneControllerData::DeathCounter)
        .property("KillCounter", &PE::EndingCutsceneControllerData::KillCounter)
        .property("RescueCounter", &PE::EndingCutsceneControllerData::RescueCounter)
        .property("TurnCounter", &PE::EndingCutsceneControllerData::TurnCounter);

    rttr::registration::class_<PE::AnimationComponent>(PE::EntityManager::GetInstance().GetComponentID<PE::AnimationComponent>().to_string().c_str())
        .method("GetAnimationID", &PE::AnimationComponent::GetAnimationID)
        .method("SetCurrentAnimationID", &PE::AnimationComponent::SetCurrentAnimationID);

    rttr::registration::class_<PE::FollowScriptData>("FollowScript")
        .property("Size", &PE::FollowScriptData::Size)
        .property("Speed", &PE::FollowScriptData::Speed)
        .property("NumberOfFollower", &PE::FollowScriptData::NumberOfFollower)
        .property("FollowingObject", &PE::FollowScriptData::FollowingObject)
        .property("rotation", &PE::FollowScriptData::Rotation)
        .property("CurrentPosition", &PE::FollowScriptData::CurrentPosition)
        .property("NextPosition", &PE::FollowScriptData::NextPosition)
        .property("ToAttach", &PE::FollowScriptData::ToAttach)
        .property("NumberOfAttacher", &PE::FollowScriptData::NumberOfAttachers)
        .property("SoundID", &PE::FollowScriptData::SoundID)
        .property("LookTowardsMovement", &PE::FollowScriptData::LookTowardsMovement)
        .property("IsAttaching", &PE::FollowScriptData::IsAttaching);

    rttr::registration::class_<PE::CameraManagerScriptData>("CameraManagerScript")
        .property("NumberOfCamera", &PE::CameraManagerScriptData::NumberOfCamera)
        .property("CameraIDs", &PE::CameraManagerScriptData::CameraIDs);

    rttr::registration::class_<PE::ObjectAttachScriptData>("ObjectAttachScript")
        .property("ObjectToAttachTo", &PE::ObjectAttachScriptData::ObjectToAttachTo);

    rttr::registration::class_<PE::CatScriptData>("CatScript")
        .property("isMainCat", &PE::CatScriptData::isMainCat)
        .property("catHealth", &PE::CatScriptData::catHealth)
        .property("catMaxEnergy", &PE::CatScriptData::catMaxEnergy)
        .property("movementSpeed", &PE::CatScriptData::movementSpeed)
        .property("attackDamage", &PE::CatScriptData::attackDamage)
        .property("requiredAttackPoints", &PE::CatScriptData::requiredAttackPoints)
        .property("bulletDelay", &PE::CatScriptData::bulletDelay)
        .property("bulletRange", &PE::CatScriptData::bulletRange)
        .property("bulletLifeTime", &PE::CatScriptData::bulletLifeTime);


    rttr::registration::class_<PE::TextComponent>(PE::EntityManager::GetInstance().GetComponentID<PE::TextComponent>().to_string().c_str())
        .property_readonly("Font", &PE::TextComponent::GetFontKey)
        .property_readonly("Size", &PE::TextComponent::GetSize)
        .property_readonly("Text", &PE::TextComponent::GetText)
        .property_readonly("Color", &PE::TextComponent::GetColor)
        .property_readonly("LineSpacing", &PE::TextComponent::GetLineSpacing)
        .property_readonly("HAlignment", &PE::TextComponent::GetHAlignment)
        .property_readonly("VAlignment", &PE::TextComponent::GetVAlignment)
        .property_readonly("HOverflow", &PE::TextComponent::GetHOverflow)
        .property_readonly("VOverflow", &PE::TextComponent::GetVOverflow)
        .method("Color", &PE::TextComponent::SetColor)
        .method("Text", &PE::TextComponent::SetText)
        .method("Size", &PE::TextComponent::SetSize)
        .method("Font", &PE::TextComponent::SetFont)
        .method("LineSpacing", &PE::TextComponent::SetLineSpacing)
        .method("HAlignment", &PE::TextComponent::SetHAlignment)
        .method("VAlignment", &PE::TextComponent::SetVAlignment)
        .method("HOverflow", &PE::TextComponent::SetHOverflow)
        .method("VOverflow", &PE::TextComponent::SetVOverflow);


    rttr::registration::class_<PE::CatScriptData>("CatScript")
        .property("catID", &PE::CatScriptData::catID)
        .property("projectileID", &PE::CatScriptData::projectileID)
        .property("catHealth", &PE::CatScriptData::catHealth)
        .property("catMaxEnergy", &PE::CatScriptData::catMaxEnergy)
        .property("attackDamage", &PE::CatScriptData::attackDamage)
        .property("requiredAttackPoints", &PE::CatScriptData::requiredAttackPoints)
        .property("bulletDelay", &PE::CatScriptData::bulletDelay)
        .property("bulletRange", &PE::CatScriptData::bulletRange)
        .property("bulletLifeTime", &PE::CatScriptData::bulletLifeTime)
        .property("bulletForce", &PE::CatScriptData::bulletForce)
        .property("animationStates", &PE::CatScriptData::animationStates);

    rttr::registration::class_<PE::CatScript_v2_0Data>("CatScript_v2_0")
        .property("catID", &PE::CatScript_v2_0Data::catID)
        .property("catType", &PE::CatScript_v2_0Data::catType)
        .property("toggleDeathAnimation", &PE::CatScript_v2_0Data::toggleDeathAnimation)
        .property("finishedExecution", &PE::CatScript_v2_0Data::finishedExecution)
        .property("isCaged", &PE::CatScript_v2_0Data::isCaged)
        .property("catMaxMovementEnergy", &PE::CatScript_v2_0Data::catMaxMovementEnergy)
        //.property("catCurrentEnergy", &PE::CatScript_v2_0Data::catCurrentEnergy)
        .property("minDistance", &PE::CatScript_v2_0Data::minDistance)
        .property("maxDistance", &PE::CatScript_v2_0Data::maxDistance)
        .property("nodeSize", &PE::CatScript_v2_0Data::nodeSize)
        .property("movementSpeed", &PE::CatScript_v2_0Data::movementSpeed)
        .property("forgivenessOffset", &PE::CatScript_v2_0Data::forgivenessOffset)
        //.property("currentPositionIndex", &PE::CatScript_v2_0Data::currentPositionIndex)
        //.property("pathPositions", &PE::CatScript_v2_0Data::pathPositions)
        //.property("followCatPositions", &PE::CatScript_v2_0Data::followCatPositions)
        //.property("pathQuads", &PE::CatScript_v2_0Data::pathQuads)
        //.property("shouldChangeState", &PE::CatScript_v2_0Data::shouldChangeState)
        //.property("delaySet", &PE::CatScript_v2_0Data::delaySet)
        //.property("timeBeforeChangingState", &PE::CatScript_v2_0Data::timeBeforeChangingState)
        .property("animationStates", &PE::CatScript_v2_0Data::animationStates)
        .property("attackVariants", &PE::CatScript_v2_0Data::attackVariables);


    rttr::registration::class_<PE::RatScriptData>("RatScript")
        .property("mainCatID", &PE::RatScriptData::mainCatID)
        .property("health", &PE::RatScriptData::health)
        .property("movementSpeed", &PE::RatScriptData::movementSpeed)
        .property("detectionRadius", &PE::RatScriptData::detectionRadius)
        .property("attackDiameter", &PE::RatScriptData::attackDiameter)
        .property("attackDuration", &PE::RatScriptData::attackDuration)
        .property("collisionDamage", &PE::RatScriptData::collisionDamage)
        .property("attackDamage", &PE::RatScriptData::attackDamage)
        .property("attackDelay", &PE::RatScriptData::attackDelay)
        .property("animationStates", &PE::RatScriptData::animationStates);

    rttr::registration::class_<PE::Canvas>(PE::EntityManager::GetInstance().GetComponentID<PE::Canvas>().to_string().c_str())
        .property_readonly("Width", &PE::Canvas::GetWidth)
        .property_readonly("Height", &PE::Canvas::GetHeight)
        .method("Width", &PE::Canvas::SetWidth)
        .method("Height", &PE::Canvas::SetHeight)
        .method("SetTargetResolution", &PE::Canvas::SetTargetResolution);

    rttr::registration::class_<PE::HealthBarScript_v2_0_Data>("HealthBarScript_v2_0")
        .property("FollowObjectID", &PE::HealthBarScript_v2_0_Data::followObjectID)
        .property("fillColorFull", &PE::HealthBarScript_v2_0_Data::fillColorFull)
        .property("fillColorHalf", &PE::HealthBarScript_v2_0_Data::fillColorHalf)
        .property("fillColorAlmostEmpty", &PE::HealthBarScript_v2_0_Data::fillColorAlmostEmpty);

    rttr::registration::class_<PE::RatScript_v2_0_Data>("RatScript_v2_0")
        .property("ratType", &PE::RatScript_v2_0_Data::ratType)
        .property("ratMaxHealth", &PE::RatScript_v2_0_Data::ratMaxHealth)
        .property("detectionRadius", &PE::RatScript_v2_0_Data::detectionRadius)
        .property("movementSpeed", &PE::RatScript_v2_0_Data::movementSpeed)
        .property("maxMovementRange", &PE::RatScript_v2_0_Data::maxMovementRange)
        .property("maxHuntTurns", &PE::RatScript_v2_0_Data::maxHuntTurns)
        .property("animationStates", &PE::RatScript_v2_0_Data::animationStates)
        .property("patrolPoints", &PE::RatScript_v2_0_Data::patrolPoints);

    rttr::registration::class_<PE::BossRatScriptData>("BossRatScript")
        .property("maxHealth", &PE::BossRatScriptData::maxHealth)
        .property("attackDelay", &PE::BossRatScriptData::attackDelay)
        .property("activationTime", &PE::BossRatScriptData::activationTime)
        .property("telegraphRadius", &PE::BossRatScriptData::telegraphRadius)
        .property("jumpSpeed", &PE::BossRatScriptData::jumpSpeed)
        .property("slamSpeed", &PE::BossRatScriptData::slamSpeed)
        .property("leftSideSlam", &PE::BossRatScriptData::leftSideSlam)
        .property("rightSideSlam", &PE::BossRatScriptData::rightSideSlam)
        .property("slamTelegraph", &PE::BossRatScriptData::slamTelegraph)
        .property("rightSideSlamAnimation", &PE::BossRatScriptData::rightSideSlamAnimation)
        .property("leftSideSlamAnimation", &PE::BossRatScriptData::leftSideSlamAnimation)
        .property("slamAreaTelegraph", &PE::BossRatScriptData::slamAreaTelegraph)
        .property("distanceBetweenPools", &PE::BossRatScriptData::distanceBetweenPools)
        .property("animationStates", &PE::BossRatScriptData::animationStates);

    rttr::registration::class_<PE::CameraShakeScriptData>("CameraShakeScript")
        .property("shakeDuration", &PE::CameraShakeScriptData::shakeDuration)
        .property("shakeAmount", &PE::CameraShakeScriptData::shakeAmount);


    rttr::registration::class_<PE::ParticleEmitter>(PE::EntityManager::GetInstance().GetComponentID<PE::ParticleEmitter>().to_string())
        .property("Is Active", &PE::ParticleEmitter::isActive)
        .property("Paused", &PE::ParticleEmitter::pause)
        .property("Type", &PE::ParticleEmitter::particleType)
        .property("Mode", &PE::ParticleEmitter::emitterType)
        .property("Emittor Length", &PE::ParticleEmitter::emittorLength)
        .property("Max particles", &PE::ParticleEmitter::maxParticles)
        .property("Emission Arc", &PE::ParticleEmitter::emissionArc)
        .property("Emission Direction", &PE::ParticleEmitter::emissionDirection)
        //.property("Emission Duration", &PE::ParticleEmitter::emissionDuration)
        //.property("Emission Rate", &PE::ParticleEmitter::emissionRate)
        .property("Looping", &PE::ParticleEmitter::isLooping)
        .property("MinMax Speed", &PE::ParticleEmitter::minMaxSpeed)
        .property("Starting Scale", &PE::ParticleEmitter::startScale)
        .property("End Scale", &PE::ParticleEmitter::endScale)
        .property("Delta Scale", &PE::ParticleEmitter::scaleChangeSpeed)
        .property("Lifetime", &PE::ParticleEmitter::startLifetime)
        .property("Color", &PE::ParticleEmitter::startColor)
        .property("Toggles", &PE::ParticleEmitter::toggles)
        //.property("", &PE::ParticleEmitter::)
        ;
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
    InitializeMemoryManager();
    InitializeSystems();
    InitializeAudio();

    //create background from file

    EntityID uiCameraId{ ResourceManager::GetInstance().LoadPrefabFromFile("EditorDefaults/Camera.prefab") };
    Graphics::CameraManager::SetUiCamera(uiCameraId);
    EntityManager::GetInstance().Get<EntityDescriptor>(uiCameraId).name = "UI Camera";

    std::srand(static_cast<unsigned>(std::time(nullptr)));
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

    // Load default assets
    ResourceManager::GetInstance().LoadDefaultAssets();

    // Set start scene
#ifndef GAMERELEASE
    SceneManager::GetInstance().CreateDefaultScene();
#else
    SceneManager::GetInstance().SetStartScene("MainMenu.scene"); // set game scene here <-
    // Load scene
    SceneManager::GetInstance().LoadScene(SceneManager::GetInstance().GetStartScene());
#endif // !GAMERELEASE

#ifdef GAMERELEASE
    // Enable the custom cursor
    Graphics::Cursor::GetInstance().SetVisibility(true);
#else
    // Disable the custom cursor
    Graphics::Cursor::GetInstance().SetVisibility(false);
#endif // GAMERELEASE

    // Set the default background colour to black
    const_cast<Graphics::RendererManager*>(GETRENDERERMANAGER())->SetBackgroundColor(0, 0, 0);

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
            m_windowManager.UpdateTitle(TimeManager::GetInstance().m_frameRateController.GetFps());
            m_lastFrameTime = currentTime;
        }

        // Update system with fixed time step
        TimeManager::GetInstance().StartAccumulator();
        while (TimeManager::GetInstance().UpdateAccumulator())
        { 
            if (!skipFrame)
            {
                for (SystemID systemID{}; systemID < SystemID::GRAPHICS; ++systemID)
                {
                    TimeManager::GetInstance().SystemStartFrame(systemID);
                    m_systemList[systemID]->UpdateSystem(TimeManager::GetInstance().GetFixedTimeStep());
                    TimeManager::GetInstance().SystemEndFrame(systemID);
                }
            }
            TimeManager::GetInstance().EndAccumulator();
        }

        Hierarchy::GetInstance().Update();
        Graphics::Cursor::GetInstance().Update(); // Update the position of the cursor texture

        
        //std::cout << Graphics::CameraManager::GetUiCameraId() << std::endl;

        // Update Graphics with variable timestep

        TimeManager::GetInstance().SystemStartFrame(SystemID::GRAPHICS);
        m_systemList[SystemID::GRAPHICS]->UpdateSystem(TimeManager::GetInstance().GetDeltaTime());
        TimeManager::GetInstance().SystemEndFrame(SystemID::GRAPHICS);

        skipFrame = false;

        // if the scene is being loaded, skip the rest of the frame
        if (SceneManager::GetInstance().IsLoadingScene())
        {
            Graphics::Cursor::GetInstance().SetCursorDefault();

            // Set the default background colour to black
            const_cast<Graphics::RendererManager*>(GETRENDERERMANAGER())->SetBackgroundColor(0, 0, 0);
            SceneManager::GetInstance().LoadSceneToLoad();
            skipFrame = true;
        }
        else if (SceneManager::GetInstance().IsRestartingScene())
        {
            Graphics::Cursor::GetInstance().SetCursorDefault();

            // Set the default background colour to black
            const_cast<Graphics::RendererManager*>(GETRENDERERMANAGER())->SetBackgroundColor(0, 0, 0);
            SceneManager::GetInstance().RestartScene(SceneManager::GetInstance().GetActiveScene());
            skipFrame = true;
        }

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
    SystemManager::GetInstance().AddSystem(system);
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
}


void PE::CoreApplication::InitializeMemoryManager()
{
    MemoryManager::GetInstance();
}

void PE::CoreApplication::InitializeSystems()
{
    // Get the window width and height to initialize the camera manager with
    int width, height;
    glfwGetWindowSize(WindowManager::GetInstance().GetWindow(), &width, &height);

    // Add system to list & assigning memory to them

    LogicSystem* p_logicSystem = new (MemoryManager::GetInstance().AllocateMemory("Logic System", sizeof(LogicSystem)))LogicSystem{};
    Graphics::CameraManager* p_cameraManager = new (MemoryManager::GetInstance().AllocateMemory("Camera Manager", sizeof(Graphics::CameraManager)))Graphics::CameraManager{ static_cast<float>(width), static_cast<float>(height) };
    Graphics::RendererManager* p_rendererManager = new (MemoryManager::GetInstance().AllocateMemory("Renderer Manager", sizeof(Graphics::RendererManager)))Graphics::RendererManager{*p_cameraManager, width, height };
    PhysicsManager* p_physicsManager = new (MemoryManager::GetInstance().AllocateMemory("Physics Manager", sizeof(PhysicsManager)))PhysicsManager{};
    CollisionManager* p_collisionManager = new (MemoryManager::GetInstance().AllocateMemory("Collision Manager", sizeof(CollisionManager)))CollisionManager{};
    InputSystem* p_inputSystem = new (MemoryManager::GetInstance().AllocateMemory("Input System", sizeof(InputSystem)))InputSystem{};
    GUISystem* p_guisystem = new (MemoryManager::GetInstance().AllocateMemory("GUI System", sizeof(GUISystem)))GUISystem{static_cast<float>(width), static_cast<float>(height)};
    AnimationManager* p_animationManager = new (MemoryManager::GetInstance().AllocateMemory("Animation System", sizeof(AnimationManager)))AnimationManager{};
    VisualEffectsManager* p_visualEffectsManager = new (MemoryManager::GetInstance().AllocateMemory("Visual Effects Manager", sizeof(VisualEffectsManager)))VisualEffectsManager{};
    //AudioManager*     p_audioManager      = new (MemoryManager::GetInstance().AllocateMemory("Audio Manager",     sizeof(AudioManager)))      AudioManager{};

    AddSystem(p_inputSystem);
    AddSystem(p_guisystem);
    AddSystem(p_logicSystem);
    AddSystem(p_physicsManager);
    AddSystem(p_collisionManager);
    AddSystem(p_animationManager);
    AddSystem(p_cameraManager);
    AddSystem(p_visualEffectsManager);
    AddSystem(p_rendererManager);
    //AddSystem(p_audioManager);

    GameStateManager::GetInstance().RegisterButtonFunctions();


}