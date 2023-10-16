#pragma once
#include "Script.h"
#include <map>
namespace PE
{
	struct TestScriptData
	{
		float m_rotationSpeed{ 10 };
	};

	class testScript : public Script
	{
	public:
		virtual void Init(EntityID id);
		virtual void Update(EntityID id, float deltaTime);
		virtual void Destroy(EntityID id);		
		virtual void OnAttach(EntityID id);
		virtual void OnDetach(EntityID id);
		std::map<EntityID, TestScriptData>& GetScriptData();
	private:
		std::map<EntityID, TestScriptData> m_ScriptData;
	};


}