#pragma once
#include "Script.h"
#include "Events/EventHandler.h"
namespace PE
{
	class PlayerControllerScript : public Script
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

	struct PlayerControllerScriptData
	{
		float speed{5000};
	};
}