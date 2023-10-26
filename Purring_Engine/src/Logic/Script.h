#pragma once
typedef unsigned long long EntityID;

namespace PE 
{
enum class ScriptState {INIT, UPDATE, EXIT};
//all the scripts will inherit this
class Script
{
public:
	Script() {}
	virtual void Init(EntityID) {}
	virtual void Update(EntityID, float) {}
	virtual void Destroy( EntityID) {}
	virtual void OnAttach(EntityID) {}
	virtual void OnDetach(EntityID) {}
	virtual ~Script() {}
};




}