/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2451-A
 \file     CatHelperFunctions.h
 \date     3-2-2024

 \author               Liew Yeni
 \par      email:      yeni.l@digipen.edu

 \brief
	This file contains declarations for functions used for a grey player cat's states.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.

*************************************************************************************/
#pragma once
#include "ECS/Entity.h"
#include "Graphics/CameraManager.h"

#define GETANIMATIONCOMPONENT(id) EntityManager::GetInstance().Get<AnimationComponent>

namespace PE
{

		struct CatHelperFunctions
		{
			/*!***********************************************************************************
			\brief Returns the position of the transform of the entity passed in.

			\param[in] transformId - ID of the entity to retrieve the position of.
			*************************************************************************************/
			static vec2 GetEntityPosition(EntityID const id)
			{
				try
				{
					Transform& r_transform{ EntityManager::GetInstance().Get<Transform>(id) };
					return r_transform.position;
				}
				catch (...) { return vec2{}; }
			}

			/*!***********************************************************************************
			 \brief Returns the scale of the transform of the entity passed in.

			 \param[in] transformId - ID of the entity to retrieve the scale of.
			*************************************************************************************/
			static vec2 GetEntityScale(EntityID const id)
			{
				try
				{
					Transform& r_transform{ EntityManager::GetInstance().Get<Transform>(id) };
					return vec2{ r_transform.width, r_transform.height };
				}
				catch (...) { return vec2{}; }
			}

			/*!***********************************************************************************
			 \brief Returns the position of the cursor in the world.

			 \return vec2 - Returns the position of the cursor in the world.
			*************************************************************************************/
			static vec2 GetCursorPositionInWorld()
			{
				float mouseX{}, mouseY{};
				InputSystem::GetCursorViewportPosition(WindowManager::GetInstance().GetWindow(), mouseX, mouseY);
				return GETCAMERAMANAGER()->GetWindowToWorldPosition(mouseX, mouseY);
			}

			/*!***********************************************************************************
			\brief Helper function to en/disables an entity.

			\param[in] id - EntityID of the entity to en/disable.
			\param[in] setToActive - Whether this entity should be set to active or inactive.
			*************************************************************************************/
			static void ToggleEntity(EntityID id, bool setToActive)
			{
				// Exit if the entity is not valid
				if (!EntityManager::GetInstance().IsEntityValid(id)) { return; }

				// Toggle the entity
				EntityManager::GetInstance().Get<EntityDescriptor>(id).isActive = setToActive;
			}

			/*!***********************************************************************************
			 \brief Adjusts the position of the transform to the value passed in.

			 \param[in] idd - ID of the entity to update the transform of.
			 \param[in] r_position - Position to set the transform to.
			*************************************************************************************/
			static void PositionEntity(EntityID const id, vec2 const& r_position)
			{
				try
				{
					Transform& r_transform{ EntityManager::GetInstance().Get<Transform>(id) }; // Get the transform of the player
					r_transform.position = r_position;
				}
				catch (...) { return; }
			}

			/*!***********************************************************************************
			 \brief Adjusts the scale of the transform to the value passed in.

			 \param[in] id - ID of the entity to update the transform of.
			 \param[in] width - Width to set the transform to.
			 \param[in] height - Height to set the transform to.
			*************************************************************************************/
			static void ScaleEntity(EntityID const id, float const width, float const height)
			{
				try
				{
					Transform& r_transform{ EntityManager::GetInstance().Get<Transform>(id) }; // Get the transform of the player
					r_transform.width = width;
					r_transform.height = height;
				}
				catch (...) { return; }
			}

			/*!***********************************************************************************
			 \brief Sets the colour of the entity.

			 \param[in] id - ID of the entity to set the colour of
			 \param[in] color - color to set the entity to
			*************************************************************************************/
			static void SetColor(EntityID const id, vec4 const& color)
			{
				try
				{
					EntityManager::GetInstance().Get<Graphics::Renderer>(id).SetColor(color.x, color.y, color.z, color.w);
				}
				catch (...) { return; }
			}

			/*!***********************************************************************************
			\brief Identifies if the entity passed in is a cat. Assumes that cats have
						the keyword "Cat" somewhere in their name in entityDescriptor.

			\param[in] id - ID of the entity to check.
			*************************************************************************************/
			static bool IsCat(EntityID const id)
			{
				return (EntityManager::GetInstance().Get<EntityDescriptor>(id).name.find("Cat") != std::string::npos);
			}

			/*!***********************************************************************************
			 \brief Identifies if the entity passed in is an enemy. Assumes that enemies have
					the keyword "Rat" somewhere in their name in entityDescriptor.

			 \param[in] id - ID of the entity to check.
			*************************************************************************************/
			static bool IsEnemy(EntityID const id)
			{
				return (EntityManager::GetInstance().Get<EntityDescriptor>(id).name.find("Rat") != std::string::npos);
			}

			/*!***********************************************************************************
			 \brief Identifies if the entity passed in is an obstacle. Assumes that obstacles
							have the keyword "Obstacle" somewhere in their name in entityDescriptor.

			 \param[in] id - ID of the entity to check.
			*************************************************************************************/
			static bool IsObstacle(EntityID const id)
			{
				return (EntityManager::GetInstance().Get<EntityDescriptor>(id).name.find("Obstacle") != std::string::npos);
			}

			/*!***********************************************************************************
			 \brief To check if the game is in cat chain level as following cats are restricted
					in this level

			 \param[out] bool - whether it is the first level in the game
			*************************************************************************************/
			
		};
	
}

