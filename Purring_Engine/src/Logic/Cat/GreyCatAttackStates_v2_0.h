/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     GreyCatAttackStates_v2_0.h
 \date     3-2-2024

 \author:              LIEW Yeni
 \par      email:      yeni.l@digipen.edu

 \brief
	This file contains the declaration for the Cat Attack Base class

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.

*************************************************************************************/
#pragma once
#include "Logic/LogicSystem.h"
#include "Logic/StateManager.h"

#include "Events/MouseEvent.h"
#include "Events/CollisionEvent.h"

#include "Logic/GameStateController_v2_0.h"
#include "CatAttackBase_v2_0.h"

namespace PE
{
	//! Enum denoting cardinal directions for attack
	enum class EnumCatAttackDirection_v2_0 : int
	{
		NONE = 0,
		EAST = 1,
		NORTH = 2,
		WEST = 3,
		SOUTH = 4
	};

	struct GreyCatAttackVariables
	{
		// id of the projectile
		EntityID projectileID{};

		// damage of the attack
		int damage{ 0 };

		// Telegraph variables
		EnumCatAttackDirection_v2_0 attackDirection{ EnumCatAttackDirection_v2_0::NONE }; // Direction of attack chosen
		std::map<EnumCatAttackDirection_v2_0, EntityID> telegraphIDs{}; // IDs of entities used to visualise the directions the player can attack in
		
		// projectile variables
		float bulletDelay{ 0.f }; // for after the bullet fire frame if slight 
		float bulletRange{ 3.f };
		float bulletLifeTime{ 1.f };
		float bulletForce{ 1000.f };

		// for syncing
		unsigned int bulletFireAnimationIndex{ 4 };
	};

	class GreyCatAttack_v2_0PLAN : public CatAttackBase_v2_0
	{
	public:
		// ----- Destructor ----- //
		~GreyCatAttack_v2_0PLAN() { p_attackData = nullptr; }

		virtual void Enter(EntityID id);

		virtual void Update(EntityID id, float deltaTime);

		virtual void CleanUp();

		virtual void Exit(EntityID id);

		virtual void ResetSelection(EntityID id);

		static void CreateProjectileTelegraphs(EntityID id, float bulletRange, std::map<EnumCatAttackDirection_v2_0, EntityID>& r_telegraphIDs);

		virtual void ToggleTelegraphs(bool setToggle, bool ignoreSelected);
		
	private:
		
		GameStateController_v2_0* p_gsc; // pointer to the game state controller
		GreyCatAttackVariables* p_attackData; // attack data for the cat

		bool m_showTelegraphs{ false }; // True if telegraphs are to be displayed

		// Telegraph colors
		vec4 const m_defaultColor{ 0.545f, 1.f, 0.576f, 1.f };
		vec4 const m_hoverColor{ 1.f, 0.859f, 0.278f, 1.f };
		vec4 const m_selectColor{ 1.f, 0.784f, 0.f, 1.f };

		bool m_rightMouseClick{ false }; // set to true when mouse is right clicked
		bool m_mouseClick{ false }; // set to true when mouse is clicked
		bool m_mouseClickedPrevious{ false }; // Set to true if the mouse was pressed in the previous frame, false otherwise
		int m_mouseClickEventListener; // Stores the handler for the mouse click event
		int m_mouseReleaseEventListener; // Stores the handler for the mouse release event

		void OnMouseClick(const Event<MouseEvents>& r_ME);
		void OnMouseRelease(const Event<MouseEvents>& r_ME);
	};

	class GreyCatAttack_v2_0EXECUTE : public State
	{
	public:
		// ----- Destructor ----- //
		virtual ~GreyCatAttack_v2_0EXECUTE() { p_attackData = nullptr; }

		virtual void StateEnter(EntityID id);

		virtual void StateUpdate(EntityID id, float deltaTime);

		virtual void StateCleanUp();

		virtual void StateExit(EntityID id);

		virtual std::string_view GetName() { return "AttackEXECUTE"; }

	private:
		
		GreyCatAttackVariables* p_attackData;

		vec2 m_bulletImpulse;
		float m_bulletDelay;
		float m_bulletLifetime; // how long the bullet will persist
		int m_collisionEventListener{};

		bool m_projectileFired{ false };
		
		void ProjectileCollided(const Event<CollisionEvents>& r_CE);

		void ProjectileHitCat(const Event<CollisionEvents>& r_CE);

		void ProjectileHitRat(const Event<CollisionEvents>& r_CE);
	};
}