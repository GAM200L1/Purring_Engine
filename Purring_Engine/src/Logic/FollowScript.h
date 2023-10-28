#pragma once
#include "Script.h"
#include "Math/MathCustom.h"

namespace PE
{
	struct FollowScriptData
	{
		EntityID Following{ 1 };
		float speed{ 100 };
		vec2 Direction;
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