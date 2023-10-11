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
	virtual void Init(EntityID id) {}
	virtual void Update(EntityID id, float deltaTime) {}
	virtual void Destroy(EntityID id) {}
};




}