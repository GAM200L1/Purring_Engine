/*!*********************************************************************************** 

 \project  Purring Engine 
 \module   CSD2401-A 
 \file     testScript.cpp 
 \date     03-11-2023

 \author               Jarran Tan Yan Zhi
 \par      email:      jarranyanzhi.tan@digipen.edu


 \brief  This file contains the definitions of functions to set, load and save game settings.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved. 

*************************************************************************************/

#include "prpch.h"
#include "Settings.h"
#include "ECS/Entity.h"

#include "GUISystem.h"
#include "AudioManager/AudioManager.h"

namespace PE 
{
	SettingsScript::SettingsScript()
	{
		std::ifstream settingsFile("../Assets/Settings/gamesettings.json");
		if (!settingsFile.fail()) {
			nlohmann::json settingsJson;
			settingsFile >> settingsJson;


			if (settingsJson.contains("settings"))
			{
				//m_firstLaunch needs to be serialized 
				if (settingsJson["settings"].contains("master"))
				{
					m_masterVol = settingsJson["settings"]["master"];
					AudioManager::GetInstance().SetMasterVolume(m_masterVol);
				}
				if (settingsJson["settings"].contains("bgm"))
				{
					m_bgm = settingsJson["settings"]["bgm"];
					AudioManager::GetInstance().SetBGMVolume(m_bgm);
				}
				if (settingsJson["settings"].contains("sfx"))
				{
					 m_sfx = settingsJson["settings"]["sfx"];
					AudioManager::GetInstance().SetSFXVolume(m_sfx);
				}
			}
			else
			{
				AudioManager::GetInstance().SetMasterVolume(1.f);
				AudioManager::GetInstance().SetBGMVolume(1.f);
				AudioManager::GetInstance().SetSFXVolume(1.f);
			}
		}
	}

	void SettingsScript::Init(EntityID id)
	{

		if (EntityManager::GetInstance().Has<GUISlider>(m_scriptData[id].MasterSlider))
		{
			EntityManager::GetInstance().Get<GUISlider>(m_scriptData[id].MasterSlider).SetSliderValue(m_masterVol);
		}

		if (EntityManager::GetInstance().Has<GUISlider>(m_scriptData[id].BGMSlider))
		{
			EntityManager::GetInstance().Get<GUISlider>(m_scriptData[id].BGMSlider).SetSliderValue(m_bgm);
		}

		if (EntityManager::GetInstance().Has<GUISlider>(m_scriptData[id].SFXSlider))
		{
			EntityManager::GetInstance().Get<GUISlider>(m_scriptData[id].SFXSlider).SetSliderValue(m_sfx);
		}

	}
	void SettingsScript::Update(EntityID id, float)
	{
		if (EntityManager::GetInstance().Has<GUISlider>(m_scriptData[id].MasterSlider))
		{
			m_masterVol = EntityManager::GetInstance().Get<GUISlider>(m_scriptData[id].MasterSlider).GetSliderValue();
			AudioManager::GetInstance().SetMasterVolume(m_masterVol);

		}

		if (EntityManager::GetInstance().Has<GUISlider>(m_scriptData[id].BGMSlider))
		{
			m_bgm = EntityManager::GetInstance().Get<GUISlider>(m_scriptData[id].BGMSlider).GetSliderValue();
			AudioManager::GetInstance().SetBGMVolume(m_bgm);
		
		}

		if (EntityManager::GetInstance().Has<GUISlider>(m_scriptData[id].SFXSlider))
		{
			m_sfx = EntityManager::GetInstance().Get<GUISlider>(m_scriptData[id].SFXSlider).GetSliderValue();
			AudioManager::GetInstance().SetSFXVolume(m_sfx);
		
		}
	}
	void SettingsScript::Destroy(EntityID)
	{
	}

	SettingsScript::~SettingsScript()
	{
		const char* filepath = "../Assets/Settings/gamesettings.json";
		std::ifstream settingsFile(filepath);
		if (!settingsFile.fail())
		{
			nlohmann::json settingsJson;
			settingsFile >> settingsJson;

			// save the stuff
			//m_firstLaunch needs to be serialized 
			settingsJson["settings"]["master"] = m_masterVol;
			settingsJson["settings"]["bgm"] = m_bgm;
			settingsJson["settings"]["sfx"] = m_sfx;

			std::ofstream outFile(filepath);
			if (outFile)
			{
				outFile << settingsJson.dump(4);
				outFile.close();
			}
			else
			{
				std::cerr << "Could not open the file for writing: " << filepath << std::endl;
			}
		}
	}

	void SettingsScript::OnAttach(EntityID id)
	{
		m_scriptData[id] = SettingsScriptData();
	}

	void SettingsScript::OnDetach(EntityID id)
	{
		auto it = m_scriptData.find(id);
		if (it != m_scriptData.end())
			m_scriptData.erase(id);
	}

	std::map<EntityID, SettingsScriptData>& SettingsScript::GetScriptData()
	{
		return m_scriptData;
	}

	rttr::instance SettingsScript::GetScriptData(EntityID id)
	{
		return rttr::instance(m_scriptData.at(id));
	}
}