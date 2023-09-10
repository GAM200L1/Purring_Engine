#pragma once
#include <string>

namespace PE
{
	class System
	{
	public:

		virtual void InitSystem() {}

		virtual void UpdateSystem() = 0;

		virtual std::string GetName() = 0;

		virtual ~System() {}		
	};
}