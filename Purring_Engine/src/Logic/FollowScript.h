#pragma once
#include "Script.h"
#include "Math/MathCustom.h"

namespace PE
{
	struct FollowScriptData
	{
		EntityID entityToFollow{ 1 };
		int numberOfFollowingEntities{ 1 };
		std::vector<EntityID> EntitiesFollowing;
		std::vector<vec2>	previousLocations;
		float distanceBeforeSaving{ 100 };
		vec2 SavedPlayerPosition;
	};


	class FollowScript : public Script
	{
	public:
		virtual void Init(EntityID) override;
		virtual void Update(EntityID,float) override;
		virtual void Destroy(EntityID) override;
		virtual void OnAttach(EntityID) override;
		virtual void OnDetach(EntityID) override;
		std::map<EntityID, FollowScriptData>& GetScriptData();
		virtual ~FollowScript();
	private:
		void LookAt(EntityID id);
	private:
		std::map<EntityID, FollowScriptData> m_data;
	};
}