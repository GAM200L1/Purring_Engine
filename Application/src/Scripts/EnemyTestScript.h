#pragma once
#include "Logic/Script.h"
#include <map>
enum EnemyState {IDLE,ALERT,PATROL,TARGET};


struct EnemyTestScriptData
{
	EnemyState EnemyCurrentState{ IDLE };
};


class EnemyTestScript : public PE::Script
{
public:
	virtual void Init(EntityID id);
	virtual void Update(EntityID id, float deltaTime);
	virtual void Destroy(EntityID id);
	virtual void OnAttach(EntityID id);
	virtual void OnDetach(EntityID id);
	virtual ~EnemyTestScript();

public:
	std::map<EntityID, EnemyTestScriptData> m_ScriptData;
};



