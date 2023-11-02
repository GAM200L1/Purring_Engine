#pragma once
#include "Script.h"
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
		rttr::instance GetScriptData(EntityID id);
		~testScript();
	private:
		std::map<EntityID, TestScriptData> m_ScriptData;
	};


}