/*!*********************************************************************************** 

 \project  Purring Engine 
 \module   CSD2401-A 
 \file     IntroCutsceneController.cpp 
 \date     7-2-2023

 \author               Brandon Ho Jun Jie
 \par      email:      brandonjunjie.ho\@digipen.edu


 \brief  This file contains the declarations of IntroCutsceneController that controls
		 the cutscene for the introduction of the game.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved. 

*************************************************************************************/

#include "prpch.h"
#include "Data/SerializationManager.h"
#include "IntroCutsceneController.h"
#include "ECS/EntityFactory.h"
#include "ECS/Entity.h"
#include "ECS/Components.h"
#include "ECS/Prefabs.h"
#include "ECS/SceneView.h"
#include "SceneManager/SceneManager.h"

#include <limits>


# define M_PI           3.14159265358979323846 

namespace PE 
{
	void IntroCutsceneController::Init(EntityID id)
	{

	}
	void IntroCutsceneController::Update(EntityID id, float deltaTime)
	{
		m_elapsedTime += deltaTime;

		if (m_startCutscene)
		{
			EntityID bgm = m_serializationManager.LoadFromFile("AudioObject/Intro Cutscene Music_Prefab.json");
			if (EntityManager::GetInstance().Has<EntityDescriptor>(bgm))
				EntityManager::GetInstance().Get<AudioComponent>(bgm).PlayAudioSound();
			EntityManager::GetInstance().RemoveEntity(bgm);
			m_startCutscene = false;
		}

		if (m_elapsedTime >= m_sceneTimer && !m_endCutscene)
		{
			EntityID bgm = m_serializationManager.LoadFromFile("AudioObject/Intro Cutscene Music_Prefab.json");
			if (EntityManager::GetInstance().Has<EntityDescriptor>(bgm))
				EntityManager::GetInstance().Get<AudioComponent>(bgm).StopSound();
			EntityManager::GetInstance().RemoveEntity(bgm);

			if (EntityManager::GetInstance().Has<EntityDescriptor>(1))
				EntityManager::GetInstance().Get<EntityDescriptor>(1).isActive = false;

			if (EntityManager::GetInstance().Has<EntityDescriptor>(4))
				EntityManager::GetInstance().Get<EntityDescriptor>(4).isActive = true;

			if (EntityManager::GetInstance().Has<EntityDescriptor>(12))
				EntityManager::GetInstance().Get<EntityDescriptor>(12).isActive = true;
			if (EntityManager::GetInstance().Has<EntityDescriptor>(14))
				EntityManager::GetInstance().Get<EntityDescriptor>(14).isActive = true;

			m_endCutscene = true;
		}

	}
	void IntroCutsceneController::Destroy(EntityID id)
	{
		EntityID bgm = m_serializationManager.LoadFromFile("AudioObject/Intro Cutscene Music_Prefab.json");
		if (EntityManager::GetInstance().Has<EntityDescriptor>(bgm))
			EntityManager::GetInstance().Get<AudioComponent>(bgm).StopSound();
		EntityManager::GetInstance().RemoveEntity(bgm);
	}

	void IntroCutsceneController::OnAttach(EntityID id)
	{
		m_ScriptData[id] = IntroCutsceneControllerData();
		m_elapsedTime = 0;
		m_endCutscene = false;
		m_startCutscene = true;
	}

	void IntroCutsceneController::OnDetach(EntityID id)
	{
		auto it = m_ScriptData.find(id);
		if (it != m_ScriptData.end())
			m_ScriptData.erase(id);
	}

	std::map<EntityID, IntroCutsceneControllerData>& IntroCutsceneController::GetScriptData()
	{
		return m_ScriptData;
	}

	rttr::instance IntroCutsceneController::GetScriptData(EntityID id)
	{
		return rttr::instance(m_ScriptData.at(id));
	}

	void IntroCutsceneController::ContinueToLevel(EntityID id)
	{
		SceneManager::GetInstance().LoadScene("Level1Scene.json");

	}

	void IntroCutsceneController::PlayClickAudio()
	{
		EntityID buttonpress = m_serializationManager.LoadFromFile("AudioObject/Button Click SFX_Prefab.json");
		if (EntityManager::GetInstance().Has<AudioComponent>(buttonpress))
			EntityManager::GetInstance().Get<AudioComponent>(buttonpress).PlayAudioSound();
		EntityManager::GetInstance().RemoveEntity(buttonpress);
	}

	IntroCutsceneController::IntroCutsceneController()
	{
		REGISTER_UI_FUNCTION(ContinueToLevel, PE::IntroCutsceneController);
	}
}