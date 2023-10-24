#pragma once
typedef unsigned long long EntityID;

namespace PE 
{

//all the scripts will inherit this
class Script
{
public:
	Script() {}
	virtual void Init(EntityID id) { id; }
	virtual void Update(EntityID id, float deltaTime) { id; deltaTime; }
	virtual void Destroy(EntityID id) { id; }
	virtual void OnAttach(EntityID id) { id; }
	virtual void OnDetach(EntityID id) { id; }
	virtual ~Script() {}
};




}