#pragma once
#include "Script.h"
#include "Events/EventHandler.h"
namespace PE
{
	class testScript2 : public Script
	{
	public:
		virtual void Init(EntityID id);
		virtual void Update(EntityID id, float deltaTime);
		virtual void Destroy(EntityID id);		
		virtual void OnAttach(EntityID id);
		virtual void OnDetach(EntityID id);
	private:
		bool Button;
	};

	struct TestScript2Data
	{
		EntityID targetID{ 1 };
		EntityID CollisionTarget{ 2 };
		std::vector<vec2> points;
	};
}