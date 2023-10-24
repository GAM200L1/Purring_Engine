#pragma once
#include "Logic/Script.h"
#include <map>
enum EnemyState {IDLE,ALERT,PATROL,TARGET};


struct EnemyTestScriptData
{
	EnemyState EnemyCurrentState{ IDLE };
	EntityID playerID{ 1 };
	float speed{ 5000 };
	float alertTimer{ 0 };
	float alertBuffer{ 3.0f };
	float patrolTimer{ 5.0f };
	bool bounce{ true };
	float idleTimer{ 3.0f };
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

private:
	void ChangeEnemyState(EntityID id, EnemyState nextState);
	float GetDistanceFromPlayer(EntityID id);
	void RotateToPlayer(EntityID id);
};



