/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401/2451-A
 \file     RatDetectionScript_v2_0_Data.h
 \date     17-01-2024

 \author               Krystal YAMIN
 \par      email:      krystal.y@digipen.edu

 \brief
	This file contains declarations for functions to detect objects within 
	the detection range of the rat.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.

*************************************************************************************/
#pragma once
#include "../LogicSystem.h"
#include "../Script.h"
#include "../ECS/Entity.h"
#include "../ECS/EntityFactory.h"
#include "../Events/EventHandler.h"
#include "../StateManager.h"

namespace PE
{
	struct RatDetectionScript_v2_0_Data
	{
		~RatDetectionScript_v2_0_Data()
		{
			delete p_stateManager;
		}

		// reference entities
		EntityID myID{ 0 };	// id of the entity with this data
		EntityID mainRatID{ 0 };	// id of the rat that this detection radius belongs to
		bool storedParentRat{false}; // True if the mainRatID has been set since this script was first initialized, false otherwise
		float detectionRadius{ 200 }; // Radius to set the collider to

		StateMachine* p_stateManager;
	};
	
	class RatDetectionScript_v2_0 : public Script
	{
		// ----- Public Members ----- //
	public:
		std::map<EntityID, RatDetectionScript_v2_0_Data> m_scriptData;

		static const int detectionColliderLayer{5}; 

		// ----- Constructors ----- //
	public:
		/*!***********************************************************************************
		 \brief Does nothing.
		*************************************************************************************/
		virtual ~RatDetectionScript_v2_0() {};


		// ----- Public Setters ----- //
	public:
		/*!***********************************************************************************
		\brief Stores the ID of the rat that this detection radius belongs to.

		\param[in,out] id - ID of the instance of the detection radius script.
		\param[in,out] ratId - ID of the rat that this detection radius belongs to.
		*************************************************************************************/
		void SetParentRat(EntityID id, EntityID ratId);

		/*!***********************************************************************************
		 \brief Set the detection radius of the rat.
		 
		 \param[in,out] id - ID of the instance of the
		 \param[in,out] newRadius - Value to set the detection radius to.
		*************************************************************************************/
		void SetDetectionRadius(EntityID id, float const newRadius);

		// ----- Public Functions ----- //
	public:
		/*!***********************************************************************************
		\brief Called when the object is first spawned into the scene

		\param[in,out] id - ID of the script to update
		*************************************************************************************/
		virtual void Init(EntityID id);

		/*!***********************************************************************************
		 \brief Called every frame.

		 \param[in,out] id - ID of the instance to update
		 \param[in,out] deltaTime - delta time used to update the state
		*************************************************************************************/
		virtual void Update(EntityID id, float deltaTime);

		/*!***********************************************************************************
		 \brief Does nothing
		*************************************************************************************/
		virtual void Destroy(EntityID) {}

		/*!***********************************************************************************
		 \brief Called when the script is attached to an entity (NOTE: even within the editor!).
				DO NOT create objects here.

		 \param[in,out] id - ID of Instance of script to update
		*************************************************************************************/
		virtual void OnAttach(EntityID id);

		/*!***********************************************************************************
		 \brief Called when the entity is destroyed and/or this script is removed from
				the entity. Delete the script data and statemanager here.

		 \param[in,out] id - ID of instance of script to clear
		*************************************************************************************/
		virtual void OnDetach(EntityID id);

		/*!***********************************************************************************
		 \brief Get the Script Data object

		 \return std::map<EntityID, RatDetectionScript_v2_0_Data>& Map of script data.
		*************************************************************************************/
		std::map<EntityID, RatDetectionScript_v2_0_Data> &GetScriptData();

		/*!***********************************************************************************
		 \brief Get the Script Data object

		 \param[in,out] id - ID of the script to get the data from
		 \return rttr::instance Instance of the script to get the data from
		*************************************************************************************/
		rttr::instance GetScriptData(EntityID id);


		// ----- Private Methods ----- // 
	private:

		/*!***********************************************************************************
		 \brief Creates the state manager if it has not been created.

		 \param[in] id EntityID of the entity to create the state manager for.
		*************************************************************************************/
		void CreateCheckStateManager(EntityID id);
	}; // end of class 



	class RatCollision_v2_0 : public State
	{
			// ----- Destructor ----- //
	public:
		virtual ~RatCollision_v2_0() override { p_data = nullptr; }

		// ----- Public Functions ----- //
	public:
		/*!***********************************************************************************
			\brief Play the idle animation and start the timer

			\param[in,out] id - ID of instance of script
		*************************************************************************************/
		virtual void StateEnter(EntityID id) override;

		/*!***********************************************************************************
			\brief Checks if its state should change

			\param[in,out] id - ID of instance of script
			\param[in,out] deltaTime - delta time to update the state with
		*************************************************************************************/
		virtual void StateUpdate(EntityID id, float deltaTime) override;

		/*!***********************************************************************************
		 \brief Unsubscribes from the collision events
		*************************************************************************************/
		virtual void StateCleanUp();

		/*!***********************************************************************************
			\brief does nothing
		*************************************************************************************/
		virtual void StateExit(EntityID id) override;

		/*!***********************************************************************************
			\brief Get the name of the state

			\return std::string_view name of state
		*************************************************************************************/
		virtual std::string_view GetName() override { return "RatCollision_v2_0"; }


		// --- COLLISION DETECTION --- // 

		/*!***********************************************************************************
		 \brief Called when a trigger enter or stay event has occured. If an event has
			occurred between this script's rat's detection collider and a cat, the parent rat
			is notified.

		 \param[in,out] r_TE - Trigger event data.
		*************************************************************************************/
		void OnTriggerEnterAndStay(const Event<CollisionEvents>& r_TE);

		/*!***********************************************************************************
		 \brief Called when a trigger exit event has occured. If an event has occurred
			between this script's rat's detection collider and a cat, the parent rat
			is notified.

		 \param[in,out] r_TE - Trigger event data.
		*************************************************************************************/
		void OnTriggerExit(const Event<CollisionEvents>& r_TE);

	private:
		RatDetectionScript_v2_0_Data* p_data; // pointer to script instance data
		// Event listener IDs 
		int m_collisionEventListener{}, m_collisionStayEventListener{}, m_collisionExitEventListener{};
	};
} // end of namespace
