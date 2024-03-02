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
namespace PE
{
	MainMenuController::MainMenuController()
	{
		REGISTER_UI_FUNCTION(PlayGameMM, PE::MainMenuController);
		REGISTER_UI_FUNCTION(QuitGameMM, PE::MainMenuController);
		REGISTER_UI_FUNCTION(ReturnFromMMAYS, PE::MainMenuController);
	}

	void MainMenuController::Init(EntityID id)
	{
		ActiveObject(EntityManager::GetInstance().Get<EntityDescriptor>(m_scriptData[id].SplashScreen).parent.value());
		m_timeSinceEnteredState = 0;
		m_timeSinceExitedState = m_splashTimer;

		//set the main menu canvas to be inactive
		ActiveObject(m_scriptData[id].MainMenuCanvas);

		//set the are you sure canvas to be inactive
		DeactiveObject(m_scriptData[id].AreYouSureCanvas);

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
					EntityID bgm = m_serializationManager.LoadFromFile("AudioObject/Menu Background Music.prefab");
					if (EntityManager::GetInstance().Has<EntityDescriptor>(bgm))
						EntityManager::GetInstance().Get<AudioComponent>(bgm).PauseSound();
					EntityManager::GetInstance().RemoveEntity(bgm);
					m_isPausedOnce = true;
					m_isResumedOnce = false;
				}
			}
			else
			{
				if (!m_isResumedOnce)
				{
					EntityID bgm = m_serializationManager.LoadFromFile("AudioObject/Menu Background Music.prefab");
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
		EntityID bgm = m_serializationManager.LoadFromFile("AudioObject/Menu Background Music.prefab");
		if (EntityManager::GetInstance().Has<EntityDescriptor>(bgm))
			EntityManager::GetInstance().Get<AudioComponent>(bgm).PauseSound();
		EntityManager::GetInstance().RemoveEntity(bgm);

		auto it = m_scriptData.find(id);
		if (it != m_scriptData.end())
		{
			REMOVE_MOUSE_EVENT_LISTENER(m_scriptData[id].mouseClickEventID);
			REMOVE_WINDOW_EVENT_LISTENER(m_scriptData[id].windowNotFocusEventID);
			REMOVE_WINDOW_EVENT_LISTENER(m_scriptData[id].windowFocusEventID);
		}


		m_firstStart = true;
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

				EntityID bgm = m_serializationManager.LoadFromFile("AudioObject/Menu Background Music.prefab");
				if (EntityManager::GetInstance().Has<EntityDescriptor>(bgm))
					EntityManager::GetInstance().Get<AudioComponent>(bgm).PlayAudioSound();
				EntityManager::GetInstance().RemoveEntity(bgm);

				ActiveObject(m_scriptData[id].MainMenuCanvas);

				m_inSplashScreen = false;
			}
		}
	}

	void MainMenuController::FadeAllObject(EntityID id, float const alpha)
	{
		//fade all the children objects first
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
		SceneManager::GetInstance().LoadScene("IntroCutsceneScene.json");
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

	void MainMenuController::PlayClickAudio()
	{
		EntityID buttonpress = m_serializationManager.LoadFromFile("AudioObject/Button Click SFX.prefab");
		if (EntityManager::GetInstance().Has<AudioComponent>(buttonpress))
			EntityManager::GetInstance().Get<AudioComponent>(buttonpress).PlayAudioSound();
		EntityManager::GetInstance().RemoveEntity(buttonpress);
	}
	void MainMenuController::PlayPositiveFeedback()
	{
		EntityID buttonpress = m_serializationManager.LoadFromFile("AudioObject/Positive Feedback.prefab");
		if (EntityManager::GetInstance().Has<AudioComponent>(buttonpress))
			EntityManager::GetInstance().Get<AudioComponent>(buttonpress).PlayAudioSound();
		EntityManager::GetInstance().RemoveEntity(buttonpress);
	}
	void MainMenuController::PlayNegativeFeedback()
	{
		EntityID buttonpress = m_serializationManager.LoadFromFile("AudioObject/Negative Feedback.prefab");
		if (EntityManager::GetInstance().Has<AudioComponent>(buttonpress))
			EntityManager::GetInstance().Get<AudioComponent>(buttonpress).PlayAudioSound();
		EntityManager::GetInstance().RemoveEntity(buttonpress);
	}
}