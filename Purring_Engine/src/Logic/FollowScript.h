#pragma once
#include "Script.h"
#include "Math/MathCustom.h"

namespace PE
{
	struct FollowScriptData
	{
		float Size{ 100 }; // fixed size of each object
		float Distance{ Size };	//distance between each object
		float Speed{ 100 };
		int NumberOfFollower{1};
		std::vector<EntityID> FollowingObject;
		float Rotation;
		vec2 CurrentPosition;
		vec2 NextPosition;
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
		std::map<EntityID, FollowScriptData> m_ScriptData;
	};
}