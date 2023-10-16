#pragma once
#include "Script.h"
#include "Events/EventHandler.h"
namespace PE
{
	struct PlayerControllerScriptData
	{
		float speed{ 5000 };
	};

	class PlayerControllerScript : public Script
	{
	public:
		virtual void Init(EntityID id);
		virtual void Update(EntityID id, float deltaTime);
		virtual void Destroy(EntityID id);		
		virtual void OnAttach(EntityID id);
		virtual void OnDetach(EntityID id);
		std::map<EntityID, PlayerControllerScriptData>& GetScriptData();
	private:
		std::map<EntityID, PlayerControllerScriptData> m_ScriptData;
	};

}