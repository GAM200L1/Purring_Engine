#pragma once
#include "Script.h"

namespace PE
{
	class testScript : public Script
	{
	public:
		testScript() {}
		virtual void Init(EntityID id);
		virtual void Update(EntityID id, float deltaTime);
		virtual void Destroy(EntityID id);		
	};

}