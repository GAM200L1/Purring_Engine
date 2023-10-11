#pragma once
#include "Script.h"

namespace PE
{
	class testScript2 : public Script
	{
	public:
		testScript2() {}
		virtual void Init(EntityID id);
		virtual void Update(EntityID id, float deltaTime);
		virtual void Destroy(EntityID id);		
	};

}