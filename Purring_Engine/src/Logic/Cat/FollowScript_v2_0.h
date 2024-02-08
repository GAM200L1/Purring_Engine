
#pragma once
#include <vector>
#include <stack>
#include "../Script.h"
#include "Math/MathCustom.h"
#include "../GameStateController_v2_0.h"
#include "Events/CollisionEvent.h"

namespace PE
{
	struct FollowScriptData_v2_0 
	{
		int Size{ 64 }; // fixed size of each object
		float Speed{ 100 };
		int NumberOfSlots{ 5 };
		std::vector<EntityID> FollowingObject;
		float Rotation;
		vec2 CurrentPosition;
		std::vector<vec2> NextPosition;

		//for attaching through code
		bool IsAttaching{ false };
		//int NumberOfAttachers{ 0 };
		std::vector<EntityID> ToAttach{};

		//look towards movement
		bool LookTowardsMovement{ false };

		// new followers stack
		std::vector<EntityID> followers;
		
		

		//sound
		EntityID SoundID;
	};


	class FollowScript_v2_0 : public Script
	{
	public:
		/*!***********************************************************************************
		\brief Does nothing
		*************************************************************************************/
		virtual void Init(EntityID) override;

		/*!***********************************************************************************
		\brief Updates the positions and rotations of the followers
		*************************************************************************************/
		virtual void Update(EntityID, float) override;

		/*!***********************************************************************************
		\brief Does nothing
		*************************************************************************************/
		virtual void Destroy(EntityID) override;

		/*!***********************************************************************************
		\brief Create followers
		*************************************************************************************/
		virtual void OnAttach(EntityID) override;
		/*!***********************************************************************************
		 \brief Clears the script data
		*************************************************************************************/
		virtual void OnDetach(EntityID) override;


		void Adopt(EntityID owner, EntityID adopt);
		void CollisionCheck(const Event<CollisionEvents>& r_event);

		/*!***********************************************************************************
		 \brief Get the Script Data object

		 \return std::map<EntityID, FollowScriptData>& Map of the script data
		*************************************************************************************/
		std::map<EntityID, FollowScriptData_v2_0>& GetScriptData();
		/*!***********************************************************************************
		 \brief Get the Script Data object

		 \param[in,out] id ID of script to return
		 \return rttr::instance Script instance
		*************************************************************************************/
		rttr::instance GetScriptData(EntityID id);
		/*!***********************************************************************************
		 \brief Does nothing
		*************************************************************************************/
		virtual ~FollowScript_v2_0();
	private:
		std::map<EntityID, FollowScriptData_v2_0> m_ScriptData;
		GameStateController_v2_0* p_gamestateController;
		// Event keys
		int m_collisionEventListener{};
	};
}