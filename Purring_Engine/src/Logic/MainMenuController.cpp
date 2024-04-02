/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     GameStateController_v2_0.cpp
 \date     15-01-2024

 \author               Jarran Tan Yan Zhi
 \par      email:      jarranyanzhi.tan@digipen.edu

 \brief
	Keep Tracks whether the game is paused or needs to be paused.
	Contains the function to unpause the game as well.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.

*************************************************************************************/

#include "prpch.h"
#include "MainMenuController.h"
#include "LogicSystem.h"
#include "ECS/Entity.h"
#include "SceneManager/SceneManager.h"
#include "Graphics/Text.h"
#include "PauseManager.h"
#include "GameStateController_v2_0.h"
#include "AudioManager/GlobalMusicManager.h"
#include "ResourceManager/ResourceManager.h"

namespace PE
{
	MainMenuController::MainMenuController()
	{
		REGISTER_UI_FUNCTION(PlayGameMM, PE::MainMenuController);
		REGISTER_UI_FUNCTION(QuitGameMM, PE::MainMenuController);
		REGISTER_UI_FUNCTION(ReturnFromMMAYS, PE::MainMenuController);
		REGISTER_UI_FUNCTION(MMCloseHTP, PE::MainMenuController);
		REGISTER_UI_FUNCTION(MMOpenHTP, PE::MainMenuController);
		REGISTER_UI_FUNCTION(MMHTPPage1, PE::MainMenuController);
		REGISTER_UI_FUNCTION(MMHTPPage2, PE::MainMenuController);
		REGISTER_UI_FUNCTION(MMOpenSettings, PE::MainMenuController);
		REGISTER_UI_FUNCTION(MMCloseSettings, PE::MainMenuController);
		REGISTER_UI_FUNCTION(PlayButtonHoverAudio, PE::MainMenuController);
		REGISTER_UI_FUNCTION(ButtonHoverExpand, PE::MainMenuController);
		REGISTER_UI_FUNCTION(ButtonOffHoverShrink, PE::MainMenuController);
	}

	void MainMenuController::Init(EntityID id)
	{
		if (EntityManager::GetInstance().Has<EntityDescriptor>(m_scriptData[id].SplashScreen) && m_firstStart)
		{
			ActiveObject(EntityManager::GetInstance().Get<EntityDescriptor>(m_scriptData[id].SplashScreen).parent.value());
			DeactiveObject(m_scriptData[id].TransitionPanel);

			m_timeSinceEnteredState = 0;
			m_timeSinceExitedState = m_splashTimer;
		}
		else
		{
			EntityID bgm = ResourceManager::GetInstance().LoadPrefabFromFile("AudioObject/Menu Background Music.prefab");
			if (EntityManager::GetInstance().Has<EntityDescriptor>(bgm))
				EntityManager::GetInstance().Get<AudioComponent>(bgm).PlayAudioSound(AudioComponent::AudioType::BGM);
			EntityManager::GetInstance().RemoveEntity(bgm);

			ActiveObject(m_scriptData[id].TransitionPanel);
		}

		//set the main menu canvas to be deactive
		m_firstStart ? DeactiveObject(m_scriptData[id].MainMenuCanvas) : ActiveObject(m_scriptData[id].MainMenuCanvas);

		//set the are you sure canvas to be inactive
		DeactiveObject(m_scriptData[id].AreYouSureCanvas);
		DeactiveObject(m_scriptData[id].HowToPlayCanvas);
		DeactiveObject(m_scriptData[id].SettingsMenu);

		//add the mouse click event listener
		m_scriptData[id].mouseClickEventID = ADD_MOUSE_EVENT_LISTENER(MouseEvents::MouseButtonPressed, MainMenuController::OnMouseClick, this);
		m_scriptData[id].windowNotFocusEventID = ADD_WINDOW_EVENT_LISTENER(PE::WindowEvents::WindowLostFocus, MainMenuController::OnWindowOutOfFocus, this)
		m_scriptData[id].windowFocusEventID = ADD_WINDOW_EVENT_LISTENER(PE::WindowEvents::WindowFocus, MainMenuController::OnWindowFocus, this)
	
		
	}

	void MainMenuController::Update(EntityID id, float deltaTime)
	{
		SplashScreenFade(id, deltaTime);

		if (!m_inSplashScreen)
		{
			if (PauseManager::GetInstance().IsPaused())
			{
				if (!m_isPausedOnce)
				{	
					EntityID bgm = ResourceManager::GetInstance().LoadPrefabFromFile("AudioObject/Menu Background Music.prefab");
					if (EntityManager::GetInstance().Has<EntityDescriptor>(bgm))
						EntityManager::GetInstance().Get<AudioComponent>(bgm).PauseSound();
					EntityManager::GetInstance().RemoveEntity(bgm);
					m_isPausedOnce = true;
					m_isResumedOnce = false;
				}
			}
			else
			{
				if (m_isTransitioning)
				{
					TransitionPanelFade(id, deltaTime, m_isTransitioningIn);
				}

				if (!m_isResumedOnce)
				{
					EntityID bgm = ResourceManager::GetInstance().LoadPrefabFromFile("AudioObject/Menu Background Music.prefab");
					if (EntityManager::GetInstance().Has<EntityDescriptor>(bgm))
						EntityManager::GetInstance().Get<AudioComponent>(bgm).ResumeSound();
					EntityManager::GetInstance().RemoveEntity(bgm);
					m_isResumedOnce = true;
					m_isPausedOnce = false;
				}

			}
		}
	}

	void MainMenuController::Destroy(EntityID id)
	{
		//stop the background music
		EntityID bgm = ResourceManager::GetInstance().LoadPrefabFromFile("AudioObject/Menu Background Music.prefab");
		if (EntityManager::GetInstance().Has<EntityDescriptor>(bgm))
			EntityManager::GetInstance().Get<AudioComponent>(bgm).PauseSound();
		EntityManager::GetInstance().RemoveEntity(bgm);

		auto it = m_scriptData.find(id);
		if (it != m_scriptData.end())
		{
			REMOVE_MOUSE_EVENT_LISTENER(m_scriptData[id].mouseClickEventID);
			REMOVE_WINDOW_EVENT_LISTENER(m_scriptData[id].windowNotFocusEventID);
			REMOVE_WINDOW_EVENT_LISTENER(m_scriptData[id].windowFocusEventID);
			m_firstStart = true;
		}

	}


	void MainMenuController::OnAttach(EntityID id)
	{
		m_scriptData[id] = MainMenuControllerData();

		m_currentMainMenuControllerEntityID = id;
	}
	void MainMenuController::OnDetach(EntityID id)
	{
		auto it = m_scriptData.find(id);
		if (it != m_scriptData.end())
			m_scriptData.erase(id);
	}
	std::map<EntityID, MainMenuControllerData>& MainMenuController::GetScriptData()
	{
		return m_scriptData;
	}
	rttr::instance MainMenuController::GetScriptData(EntityID id)
	{
		return rttr::instance(m_scriptData.at(id));
	}

	void MainMenuController::OnMouseClick(const Event<MouseEvents>& r_ME)
	{
		MouseButtonPressedEvent MBPE;
		MBPE = dynamic_cast<const MouseButtonPressedEvent&>(r_ME);

		if (MBPE.button == GLFW_MOUSE_BUTTON_RIGHT)
		{
			if (m_inSplashScreen)
			{
				m_timeSinceEnteredState = m_splashTimer;

			}
		}
	}

	void MainMenuController::OnWindowOutOfFocus(const PE::Event<PE::WindowEvents>& r_event)
	{
		PauseManager::GetInstance().SetPaused(true);
	}

	void MainMenuController::OnWindowFocus(const PE::Event<PE::WindowEvents>& r_event)
	{
		PauseManager::GetInstance().SetPaused(false);
	}

	void MainMenuController::NotFirstStart()
	{
		m_firstStart = false;

		m_isTransitioning = true;
		m_isTransitioningIn = true;
	}

	void MainMenuController::ActiveObject(EntityID id)
	{
		if (!EntityManager::GetInstance().Has<EntityDescriptor>(id))
			return;

		//set active the current object
		EntityManager::GetInstance().Get<EntityDescriptor>(id).isActive = true;

		//set active the childrens if there are any
		for (auto id2 : EntityManager::GetInstance().Get<EntityDescriptor>(id).children)
		{
			if (EntityManager::GetInstance().Has<GUIButton>(id2))
			{
				EntityManager::GetInstance().Get<GUIButton>(id2).disabled = false;
			}

			//set active the children of the childrens, up to only 2nd child level
			for (auto id3 : EntityManager::GetInstance().Get<EntityDescriptor>(id2).children)
			{
				if (EntityManager::GetInstance().Has<GUIButton>(id3))
				{
					EntityManager::GetInstance().Get<GUIButton>(id3).disabled = false;
				}

				if (!EntityManager::GetInstance().Has<EntityDescriptor>(id3))
					break;

				EntityManager::GetInstance().Get<EntityDescriptor>(id3).isActive = true;
			}
			if (!EntityManager::GetInstance().Has<EntityDescriptor>(id2))
				break;

			EntityManager::GetInstance().Get<EntityDescriptor>(id2).isActive = true;
		}
	}

	void MainMenuController::DeactiveObject(EntityID id)
	{
		//deactive all the children objects first
		if(EntityManager::GetInstance().Has<EntityDescriptor>(id))
		for (auto id2 : EntityManager::GetInstance().Get<EntityDescriptor>(id).children)
		{
			if (!EntityManager::GetInstance().Has<EntityDescriptor>(id2))
				break;

			EntityManager::GetInstance().Get<EntityDescriptor>(id2).isActive = false;
		}

		if (!EntityManager::GetInstance().Has<EntityDescriptor>(id))
			return;

		//deactive current object
		EntityManager::GetInstance().Get<EntityDescriptor>(id).isActive = false;
	}

	void MainMenuController::SplashScreenFade(EntityID const id, float deltaTime)
	{
		if (!m_inSplashScreen && m_firstStart) //replace with boolean
		{
			if (EntityManager::GetInstance().Has<EntityDescriptor>(m_scriptData[id].SplashScreen))
			ActiveObject(EntityManager::GetInstance().Get<EntityDescriptor>(m_scriptData[id].SplashScreen).parent.value());
			m_firstStart = false;
			m_inSplashScreen = true;
			m_timeSinceEnteredState = 0;
			m_timeSinceExitedState = m_splashTimer;
			DeactiveObject(m_scriptData[id].MainMenuCanvas);
		}

		if (m_inSplashScreen)
		{
			m_timeSinceEnteredState += deltaTime;
			m_timeSinceExitedState -= deltaTime;

			float fadeOutSpeed = std::clamp(m_timeSinceExitedState / m_splashTimer, 0.0f, 1.0f);
			float fadeInSpeed = std::clamp(m_timeSinceEnteredState / m_splashTimer, 0.0f, 1.0f);

			FadeAllObject(m_scriptData[id].SplashScreen, fadeOutSpeed);


			if (fadeInSpeed >= 1)
			{
				DeactiveObject(EntityManager::GetInstance().Get<EntityDescriptor>(m_scriptData[id].SplashScreen).parent.value());
				EntityManager::GetInstance().Get<EntityDescriptor>(EntityManager::GetInstance().Get<EntityDescriptor>(m_scriptData[id].SplashScreen).parent.value()).isActive = false;

				EntityID bgm = ResourceManager::GetInstance().LoadPrefabFromFile("AudioObject/Menu Background Music.prefab");
				if (EntityManager::GetInstance().Has<EntityDescriptor>(bgm))
					EntityManager::GetInstance().Get<AudioComponent>(bgm).PlayAudioSound(AudioComponent::AudioType::BGM);
				EntityManager::GetInstance().RemoveEntity(bgm);

				ActiveObject(m_scriptData[id].MainMenuCanvas);

				m_inSplashScreen = false;
			}
		}
	}

	void MainMenuController::TransitionPanelFade(EntityID const id, float deltaTime, bool in)
	{
		ActiveObject(m_scriptData[id].TransitionPanel);

		m_timeSinceTransitionStarted += deltaTime;
		m_timeSinceTransitionEnded -= deltaTime;

		float fadeOutSpeed = std::clamp(m_timeSinceTransitionEnded / m_transitionTimer, 0.0f, 1.0f);
		float fadeInSpeed = std::clamp(m_timeSinceTransitionStarted / m_transitionTimer, 0.0f, 1.0f);

		FadeAllObject(m_scriptData[id].TransitionPanel, in ? fadeOutSpeed : fadeInSpeed);

		if (fadeInSpeed >= 1 && !in)
		{
			m_isTransitioning = false;
			m_isTransitioningIn = true;
			GETSCRIPTINSTANCEPOINTER(GameStateController_v2_0)->SetCurrentLevel(0);
			GETSCRIPTINSTANCEPOINTER(GameStateController_v2_0)->gameEnded = false;
			SceneManager::GetInstance().LoadScene("IntroCutsceneScene.scene");

			m_timeSinceTransitionStarted = 0;
			m_timeSinceTransitionEnded = m_transitionTimer;
		}
		else if (fadeOutSpeed >= 1 && in)
		{
			m_isTransitioning = false;
			m_isTransitioningIn = false;
			DeactiveObject(m_scriptData[m_currentMainMenuControllerEntityID].TransitionPanel);

			m_timeSinceTransitionStarted = 0;
			m_timeSinceTransitionEnded = m_transitionTimer;
		}
		
	}

	void MainMenuController::FadeAllObject(EntityID id, float const alpha)
	{
		//fade all the children objects first
		if (!EntityManager::GetInstance().Has<EntityDescriptor>(id))
			return;

		for (auto id2 : EntityManager::GetInstance().Get<EntityDescriptor>(id).children)
		{
			if (EntityManager::GetInstance().Has<TextComponent>(id2))
			{
				EntityManager::GetInstance().Get<TextComponent>(id2).SetAlpha(alpha);
			}
			else if (EntityManager::GetInstance().Has<Graphics::GUIRenderer>(id2))
			{
				EntityManager::GetInstance().Get<Graphics::GUIRenderer>(id2).SetAlpha(alpha);
			}
		}

		if (!EntityManager::GetInstance().Has<Graphics::GUIRenderer>(id))
			return;

		//fade the current object
		EntityManager::GetInstance().Get<Graphics::GUIRenderer>(id).SetAlpha(alpha);
	}

	void MainMenuController::PlayGameMM(EntityID)
	{
		m_isTransitioning = true;
		m_isTransitioningIn = false;
		PlayPositiveFeedback();
	}

	void MainMenuController::QuitGameMM(EntityID)
	{
		ActiveObject(m_scriptData[m_currentMainMenuControllerEntityID].AreYouSureCanvas);
		DeactiveObject(m_scriptData[m_currentMainMenuControllerEntityID].MainMenuCanvas);
		PlayNegativeFeedback();
	}

	void MainMenuController::ReturnFromMMAYS(EntityID)
	{
		ActiveObject(m_scriptData[m_currentMainMenuControllerEntityID].MainMenuCanvas);
		DeactiveObject(m_scriptData[m_currentMainMenuControllerEntityID].AreYouSureCanvas);
		PlayClickAudio();
	}

	void MainMenuController::MMCloseHTP(EntityID)
	{
		DeactiveObject(m_scriptData[m_currentMainMenuControllerEntityID].HowToPlayCanvas);
		ActiveObject(m_scriptData[m_currentMainMenuControllerEntityID].MainMenuCanvas);
		PlayClickAudio();
	}

	void MainMenuController::MMOpenHTP(EntityID)
	{
		ActiveObject(m_scriptData[m_currentMainMenuControllerEntityID].HowToPlayCanvas);
		DeactiveObject(m_scriptData[m_currentMainMenuControllerEntityID].MainMenuCanvas);
		DeactiveObject(m_scriptData[m_currentMainMenuControllerEntityID].HowToPlayPageTwo);
		for (auto id2 : EntityManager::GetInstance().Get<EntityDescriptor>(m_scriptData[m_currentMainMenuControllerEntityID].HowToPlayCanvas).children)
		{
			if (EntityManager::GetInstance().Get<EntityDescriptor>(id2).name == "pg1")
			{
				EntityManager::GetInstance().Get<EntityDescriptor>(id2).isActive = false;
			}
		}
		PlayClickAudio();
	}

	void MainMenuController::MMHTPPage2(EntityID)
	{
		ActiveObject(m_scriptData[m_currentMainMenuControllerEntityID].HowToPlayPageTwo);
		DeactiveObject(m_scriptData[m_currentMainMenuControllerEntityID].HowToPlayPageOne);

		for (auto id2 : EntityManager::GetInstance().Get<EntityDescriptor>(m_scriptData[m_currentMainMenuControllerEntityID].HowToPlayCanvas).children)
		{
			if (EntityManager::GetInstance().Get<EntityDescriptor>(id2).name == "pg1")
			{
				EntityManager::GetInstance().Get<EntityDescriptor>(id2).isActive = true;
			}
			else if (EntityManager::GetInstance().Get<EntityDescriptor>(id2).name == "pg2")
			{
				EntityManager::GetInstance().Get<EntityDescriptor>(id2).isActive = false;
			}
		}
		PlayClickAudio();
	}

	void MainMenuController::MMHTPPage1(EntityID)
	{
		ActiveObject(m_scriptData[m_currentMainMenuControllerEntityID].HowToPlayPageOne);
		DeactiveObject(m_scriptData[m_currentMainMenuControllerEntityID].HowToPlayPageTwo);

		for (auto id2 : EntityManager::GetInstance().Get<EntityDescriptor>(m_scriptData[m_currentMainMenuControllerEntityID].HowToPlayCanvas).children)
		{
			if (EntityManager::GetInstance().Get<EntityDescriptor>(id2).name == "pg1")
			{
				EntityManager::GetInstance().Get<EntityDescriptor>(id2).isActive = false;
			}
			else if (EntityManager::GetInstance().Get<EntityDescriptor>(id2).name == "pg2")
			{
				EntityManager::GetInstance().Get<EntityDescriptor>(id2).isActive = true;
			}
		}
		PlayClickAudio();
	}

	void MainMenuController::MMOpenSettings(EntityID)
	{
		ActiveObject(m_scriptData[m_currentMainMenuControllerEntityID].SettingsMenu);
		DeactiveObject(m_scriptData[m_currentMainMenuControllerEntityID].MainMenuCanvas);
	}


	void MainMenuController::MMCloseSettings(EntityID)
	{
		ActiveObject(m_scriptData[m_currentMainMenuControllerEntityID].MainMenuCanvas);
		DeactiveObject(m_scriptData[m_currentMainMenuControllerEntityID].SettingsMenu);
	
	}

	void MainMenuController::PlayClickAudio()
	{
		EntityID buttonpress = ResourceManager::GetInstance().LoadPrefabFromFile("AudioObject/Button Click SFX.prefab");
		if (EntityManager::GetInstance().Has<AudioComponent>(buttonpress))
			EntityManager::GetInstance().Get<AudioComponent>(buttonpress).PlayAudioSound(AudioComponent::AudioType::SFX);
		EntityManager::GetInstance().RemoveEntity(buttonpress);
	}
	void MainMenuController::PlayPositiveFeedback()
	{
		EntityID buttonpress = ResourceManager::GetInstance().LoadPrefabFromFile("AudioObject/Positive Feedback.prefab");
		if (EntityManager::GetInstance().Has<AudioComponent>(buttonpress))
			EntityManager::GetInstance().Get<AudioComponent>(buttonpress).PlayAudioSound(AudioComponent::AudioType::SFX);
		EntityManager::GetInstance().RemoveEntity(buttonpress);
	}
	void MainMenuController::PlayNegativeFeedback()
	{
		EntityID buttonpress = ResourceManager::GetInstance().LoadPrefabFromFile("AudioObject/Negative Feedback.prefab");
		if (EntityManager::GetInstance().Has<AudioComponent>(buttonpress))
			EntityManager::GetInstance().Get<AudioComponent>(buttonpress).PlayAudioSound(AudioComponent::AudioType::SFX);
		EntityManager::GetInstance().RemoveEntity(buttonpress);
	}

	void MainMenuController::PlayButtonHoverAudio(EntityID)
	{
		GlobalMusicManager::GetInstance().PlaySFX("AudioObject/UIButtonHoverSFX.prefab",false);
	}

	void MainMenuController::ButtonHoverExpand(EntityID id)
	{
		PlayButtonHoverAudio(id);

		if (EntityManager::GetInstance().Has<Transform>(id))
		{
			Transform* buttonTransform = &EntityManager::GetInstance().Get<Transform>(id);
			buttonTransform->width *= 1.1f;
			buttonTransform->height *= 1.1f;
		}
	}

	void MainMenuController::ButtonOffHoverShrink(EntityID id)
	{
		if (EntityManager::GetInstance().Has<Transform>(id))
		{
			Transform* buttonTransform = &EntityManager::GetInstance().Get<Transform>(id);
			buttonTransform->width = buttonTransform->width/11.f * 10.f;
			buttonTransform->height = buttonTransform->height/11.f * 10.f;
		}

	}
}