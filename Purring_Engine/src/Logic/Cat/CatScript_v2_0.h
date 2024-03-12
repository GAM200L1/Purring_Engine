/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2451-A
 \file     CatScript_v2_0.h
 \date     15-1-2024

 \author               Liew Yeni
 \par      email:      yeni.l@digipen.edu

 \brief
	This file contains the structs and declarations of functions for the base of a cat object.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.

*************************************************************************************/
#pragma once
#include <variant>
#include "ECS/Entity.h"

#include "Logic/Script.h"
#include "Logic/LogicSystem.h"
#include "Logic/StateManager.h"
#include "Animation/Animation.h"

#include "Logic/GameStateController_v2_0.h"
#include "GreyCatAttackStates_v2_0.h"
#include "OrangeCatAttackStates_v2_0.h"

namespace PE
{
	enum EnumCatType
	{
		MAINCAT,
		GREYCAT,
		ORANGECAT
		//FLUFFYCAT
	};

	struct CatScript_v2_0Data
	{
		~CatScript_v2_0Data()
		{
			delete p_stateManager;
		}

		// id of cat and its type
		EntityID catID{ 0 };
		EnumCatType catType{ EnumCatType::ORANGECAT };
		bool finishedExecution{ false };
		bool toggleDeathAnimation{ false };
		bool isCaged{ false };

		// cat attack
		std::variant<GreyCatAttackVariables, OrangeCatAttackVariables> attackVariables{ GreyCatAttackVariables{} };
		bool attackSelected{ false };

		// movement variables
		int catMaxMovementEnergy{ 21 };
		int catCurrentEnergy{ catMaxMovementEnergy };

		float minDistance{ 10.f }; float maxDistance{ 50.f }; // Min and max distance enforced between each path node
		float nodeSize{ 10.f }; // Size (in pixels) of each node
		float movementSpeed{ 300.f }; // Speed to move the cat along the path
		float forgivenessOffset{ 1.f }; // Amount that the cat can be offset from the path node before attempting to move to the next one
		unsigned currentPositionIndex{}; // Index of the position in the pathPositions container that the cat should move towards
		
		// planning variables
		

		// movement variables
		std::vector<vec2> pathPositions{}; // Positions of the nodes of the player drawn path
		std::vector<EntityID> pathQuads{}; // IDs of entities to visualise the path nodes
		vec2 resetPosition{ 0.f,0.f };

		// state manager
		StateMachine* p_stateManager{ nullptr }; // Cat state manager. Deleted in destructor.
		bool shouldChangeState{};  // Flags that the state should change when [timeBeforeChangingState] is zero
		bool delaySet{ false }; // Whether the state change has been flagged
		float timeBeforeChangingState{ 0.f }; // Delay before state should change
		bool planningAttack{ false };

		// animation
		std::map<std::string, std::string> animationStates; // animation states of the cat <name, file>
		bool executionAnimationFinished{ false };
		bool playDeathSound{ true };
	};

	class CatScript_v2_0 : public Script
	{
	public:
		// ----- Public Variables ----- //
		std::map<EntityID, CatScript_v2_0Data> m_scriptData; // data associated with each instance of the script
		
	public:

		// ----- Destructor ----- //
		virtual ~CatScript_v2_0();

		/*!***********************************************************************************
		\brief					The Initialise Function for a script, to initialize any variables
		\param [In] EntityID	The ID of the object currently running the script
		*************************************************************************************/
		virtual void Init(EntityID id);

		/*!***********************************************************************************
		\brief					The update function of the script
		\param [In] EntityID	The ID of the object currently running the script
		*************************************************************************************/
		virtual void Update(EntityID id, float deltaTime);

		/*!***********************************************************************************
		\brief					Does nothing
		*************************************************************************************/
		virtual void Destroy(EntityID id){}

		/*!***********************************************************************************
		\brief					Adds needed components to object with this script
		\param [In] EntityID	The ID of the object currently running the script
		*************************************************************************************/
		virtual void OnAttach(EntityID id);
		
		/*!***********************************************************************************
		\brief					Removes this object from scriptData
		\param [In] EntityID	The ID of the object currently running the script
		*************************************************************************************/
		virtual void OnDetach(EntityID id);


		// ----- Getters/Setters/RTTR ----- //

		/*!***********************************************************************************
		 \brief Sets the flag for the state to be changed after the delay passed in.

		 \param[in] id - EntityID of the entity to change the state of.
		 \param[in] stateChangeDelay - Time in seconds before switching to the next state.
										Set to zero by default.
		*************************************************************************************/
		void TriggerStateChange(EntityID id, float const stateChangeDelay = 0.f);
		
		/*!***********************************************************************************
		 \brief Returns the container of script data.
		*************************************************************************************/
		std::map<EntityID, CatScript_v2_0Data>& GetScriptData() { return m_scriptData; }

		/*!***********************************************************************************
		 \brief Returns the container of script data.
		*************************************************************************************/
		rttr::instance GetScriptData(EntityID id) { return rttr::instance(m_scriptData.at(id)); }

		// ----- Audio Helper Functions ----- //
		/*!***********************************************************************************
		 \brief Plays the death audio depending on the cat type
		*************************************************************************************/
		static void PlayDeathAudio(EnumCatType catType);

		/*!***********************************************************************************
		 \brief Plays the path placement audio during movement planning
		*************************************************************************************/
		static void PlayPathPlacementAudio();
		
		/*!***********************************************************************************
		 \brief Plays the footstep audio
		*************************************************************************************/
		static void PlayFootstepAudio();

		/*!***********************************************************************************
		 \brief Plays the attack audio depending on the cat type
		*************************************************************************************/
		static void PlayCatAttackAudio(EnumCatType catType);

		/*!***********************************************************************************
		 \brief Plays the rescure audio depending on the cat type
		*************************************************************************************/
		static void PlayRescueCatAudio(EnumCatType catType);

	private:
		// ----- Private Variables ----- //
		GameStateController_v2_0* p_gsc = nullptr;

		// ID of the main cat
		EntityID m_mainCatID;

		// Event Listeners
		int m_mouseClickEventListener{};
		int m_mouseReleaseEventListener{};

		// mouse click
		bool m_mouseClick{ false };
		bool m_mouseClickPrevious{ false };

	private:
		// ----- Private Functions ----- //

		/*!***********************************************************************************
		 \brief Creates a path node to visualise the path drawn by the player.

		 \param[in] id EntityID of the entity that this script is attached to.
		*************************************************************************************/
		void CreatePathNode(EntityID id);
		
		/*!***********************************************************************************
		 \brief Creates a statemanager and sets the starting state. If the statemanager exists,
						don't bother making a new one.

		 \param[in] id - EntityID of the entity to en/disable.
		 \param[in] setToActive - Whether this entity should be set to active or inactive.
		*************************************************************************************/
		void MakeStateManager(EntityID id);

		/*!***********************************************************************************
		 \brief Changes the state if the flag has been set and the delay is zero and below.

		 \param[in] id - EntityID of the entity to change the state of.
		 \param[in] deltaTime - Time in seconds that have passed since the last frame.

		 \return true - True if the state should change.
		 \return false - False if the state should NOT change.
		*************************************************************************************/
		bool CheckShouldStateChange(EntityID id, float const deltaTime);

		/*!***********************************************************************************
		 \brief Helper function for cat planning state

		 \param[in] id - EntityID of the cat undergoing the planning state.
		 \param[in] deltaTime - Time in seconds that have passed since the last frame.
		*************************************************************************************/
		template <typename AttackPLAN>
		void PlanningStatesHandler(EntityID id, float deltaTime);

		/*!***********************************************************************************
		 \brief Helper function for cat execution state

		 \param[in] id - EntityID of the cat undergoing the execution state.
		 \param[in] deltaTime - Time in seconds that have passed since the last frame.
		*************************************************************************************/
		template <typename AttackEXECUTE>
		void ExecuteStatesHandler(EntityID id, float deltaTime);

		/*!***********************************************************************************
		 \brief Helper function for playing cat animation.

		 \param[in] id - EntityID of the cat which the animation should be playing on.
		 \param[in] r_animationState - animation state that should be played.
		*************************************************************************************/
		void PlayAnimation(EntityID id, std::string const& r_animationState);

		/*!***********************************************************************************
		 \brief Helper function for cat selection

		 \param[in] r_ME - mouse selection trigger data.
		*************************************************************************************/
		void OnMouseClick(const Event<MouseEvents>& r_ME);

		/*!***********************************************************************************
		 \brief Helper function for cat state change

		 \param[in] id - EntityID of the cat undergoing the execution state.
		*************************************************************************************/
		inline void ChangeToPlanningState(EntityID id);
	};
}