#pragma once
#include <string>

namespace PE
{
	class System
	{
	public:

		virtual ~System() {}

		virtual void InitializeSystem() {}

		virtual void UpdateSystem(float deltaTime) = 0;

		virtual void DestroySystem() = 0;

		virtual std::string GetName() = 0;
	};
}