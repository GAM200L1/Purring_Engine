#include "prpch.h"
#include "PlayerControllerScript.h"
#include "Input/InputSystem.h"
# define M_PI           3.14159265358979323846 

namespace PE 
{

	void PlayerControllerScript::Init(EntityID id)
	{
	}
	void PlayerControllerScript::Update(EntityID id, float deltaTime)
	{
		// Movement
		if (InputSystem::IsKeyHeld(GLFW_KEY_W))
		{
			EntityManager::GetInstance().Get<RigidBody>(id).ApplyForce(vec2{ 0.f,1.f } *PE::EntityManager::GetInstance().Get <PlayerControllerScriptData>(id).speed);
		}
		if (InputSystem::IsKeyHeld(GLFW_KEY_A))
		{
			EntityManager::GetInstance().Get<RigidBody>(id).ApplyForce(vec2{ -1.f,0.f } *PE::EntityManager::GetInstance().Get <PlayerControllerScriptData>(id).speed);
		}
		if (InputSystem::IsKeyHeld(GLFW_KEY_S))
		{
			EntityManager::GetInstance().Get<RigidBody>(id).ApplyForce(vec2{ 0.f,-1.f } *PE::EntityManager::GetInstance().Get <PlayerControllerScriptData>(id).speed);
		}
		if (InputSystem::IsKeyHeld(GLFW_KEY_D))
		{
			EntityManager::GetInstance().Get<RigidBody>(id).ApplyForce(vec2{ 1.f,0.f } *PE::EntityManager::GetInstance().Get <PlayerControllerScriptData>(id).speed);
		}
	}
	void PlayerControllerScript::Destroy(EntityID id)
	{
	}

	void PlayerControllerScript::OnAttach(EntityID id)
	{
		if (!EntityManager::GetInstance().Has(id, "RigidBody"))
			EntityFactory::GetInstance().Assign(id, { "RigidBody" });

		if (!EntityManager::GetInstance().Has(id, "PlayerControllerScriptData"))
			EntityFactory::GetInstance().Assign(id, { "PlayerControllerScriptData" });


	}

	void PlayerControllerScript::OnDetach(EntityID id)
	{
	}

}