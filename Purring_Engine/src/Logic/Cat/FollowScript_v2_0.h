
#pragma once
#include <vector>
#include "../Script.h"
#include "Math/MathCustom.h"

namespace PE
{
	struct FollowScriptData_v2_0 
	{
		int Size{ 100 }; // fixed size of each object
		float Speed{ 100 };
		int NumberOfFollower{ 1 };
		std::vector<EntityID> FollowingObject;
		float Rotation;
		vec2 CurrentPosition;
		std::vector<vec2> NextPosition;

		//for attaching through code
		bool IsAttaching{ false };
		int NumberOfAttachers{ 1 };
		std::vector<EntityID> ToAttach{};

		//look towards movement
		bool LookTowardsMovement{ true };

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
	};
}