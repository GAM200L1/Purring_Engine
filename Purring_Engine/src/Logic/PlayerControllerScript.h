#pragma once
#include "Script.h"
#include "Events/EventHandler.h"
namespace PE
{
	enum class  PlayerState{ IDLE = 0, MOVING, DEAD };

	struct PlayerControllerScriptData
	{
		PlayerState currentPlayerState{ PlayerState::IDLE };
		int HP{ 100 };
		float speed{ 100 };
	};

	class PlayerControllerScript : public Script
	{
	public:
		virtual void Init(EntityID id);
		virtual void Update(EntityID id, float deltaTime);
		virtual void Destroy(EntityID id);		
		virtual void OnAttach(EntityID id);
		virtual void OnDetach(EntityID id);
		void MovePlayer(EntityID id,float deltaTime);
		void CheckState(EntityID id);
		std::map<EntityID, PlayerControllerScriptData>& GetScriptData();
		rttr::instance GetScriptData(EntityID id);
		~PlayerControllerScript();
	private:
		void CollisionEnter(const Event<CollisionEvents>& r_e);
	private:
		std::map<EntityID, PlayerControllerScriptData> m_ScriptData;
	};

}