#pragma once
#include "Script.h"
#include "Events/EventHandler.h"
namespace PE
{
	enum PlayerState { IDLE, MOVING, DEAD };

	struct PlayerControllerScriptData
	{
		PlayerState currentPlayerState{ IDLE };
		int HP{ 100 };
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
		void MovePlayer(EntityID id);
		void CheckState(EntityID id);
		std::map<EntityID, PlayerControllerScriptData>& GetScriptData();
		~PlayerControllerScript();
	private:
		std::map<EntityID, PlayerControllerScriptData> m_ScriptData;
	};

}