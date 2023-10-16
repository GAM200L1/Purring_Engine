#pragma once
#include "ECS/EntityFactory.h"
#include "ECS/Entity.h"
#include "ECS/Components.h"
#include "ECS/Prefabs.h"
#include "ECS/SceneView.h"
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