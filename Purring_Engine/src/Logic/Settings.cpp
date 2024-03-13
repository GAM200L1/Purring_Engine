/*!*********************************************************************************** 

 \project  Purring Engine 
 \module   CSD2401-A 
 \file     testScript.cpp 
 \date     03-11-2023

 \author               Jarran Tan Yan Zhi
 \par      email:      jarranyanzhi.tan@digipen.edu


 \brief  This file contains the definitions of testScript

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
					float MasterVolume = settingsJson["settings"]["master"];
					AudioManager::GetInstance().SetMasterVolume(MasterVolume);
				}
				if (settingsJson["settings"].contains("bgm"))
				{
					float BGMVolume = settingsJson["settings"]["bgm"];
					AudioManager::GetInstance().SetBGMVolume(BGMVolume);
				}
				if (settingsJson["settings"].contains("sfx"))
				{
					float SFXVolume = settingsJson["settings"]["sfx"];
					AudioManager::GetInstance().SetSFXVolume(SFXVolume);
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
			EntityManager::GetInstance().Get<GUISlider>(m_scriptData[id].MasterSlider).SetSliderValue(AudioManager::GetInstance().GetMasterVolume());
		}

		if (EntityManager::GetInstance().Has<GUISlider>(m_scriptData[id].BGMSlider))
		{
			EntityManager::GetInstance().Get<GUISlider>(m_scriptData[id].BGMSlider).SetSliderValue(AudioManager::GetInstance().GetBGMVolume());
		}

		if (EntityManager::GetInstance().Has<GUISlider>(m_scriptData[id].SFXSlider))
		{
			EntityManager::GetInstance().Get<GUISlider>(m_scriptData[id].SFXSlider).SetSliderValue(AudioManager::GetInstance().GetSFXVolume());
		}

	}
	void SettingsScript::Update(EntityID id, float)
	{
		if (EntityManager::GetInstance().Has<GUISlider>(m_scriptData[id].MasterSlider))
		{
			AudioManager::GetInstance().SetMasterVolume(EntityManager::GetInstance().Get<GUISlider>(m_scriptData[id].MasterSlider).GetSliderValue());
		}

		if (EntityManager::GetInstance().Has<GUISlider>(m_scriptData[id].BGMSlider))
		{
			AudioManager::GetInstance().SetBGMVolume(EntityManager::GetInstance().Get<GUISlider>(m_scriptData[id].BGMSlider).GetSliderValue());
		
		}

		if (EntityManager::GetInstance().Has<GUISlider>(m_scriptData[id].SFXSlider))
		{
			AudioManager::GetInstance().SetSFXVolume(EntityManager::GetInstance().Get<GUISlider>(m_scriptData[id].SFXSlider).GetSliderValue());
		
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
			settingsJson["settings"]["master"] = AudioManager::GetInstance().GetMasterVolume();
			settingsJson["settings"]["bgm"] = AudioManager::GetInstance().GetBGMVolume();
			settingsJson["settings"]["sfx"] = AudioManager::GetInstance().GetSFXVolume();


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