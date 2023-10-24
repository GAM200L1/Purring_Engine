#pragma once
#include "Script.h"
#include "Math/MathCustom.h"

namespace PE
{
	struct TestScript2Data
	{
		EntityID targetID{ 1 };
		EntityID CollisionTarget{ 2 };
		std::vector<vec2> points;
	};

	class testScript2 : public Script
	{
	public:
		virtual void Init(EntityID id);
		virtual void Update(EntityID id, float deltaTime);
		virtual void Destroy(EntityID id);		
		virtual void OnAttach(EntityID id);
		virtual void OnDetach(EntityID id);
		~testScript2();
	private:
		std::map<EntityID, TestScript2Data> m_ScriptData;
	};

}