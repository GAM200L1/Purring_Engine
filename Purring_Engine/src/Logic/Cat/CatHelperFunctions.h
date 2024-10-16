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
				if (EntityManager::GetInstance().Has<Transform>(id))
				{
					Transform& r_transform{ EntityManager::GetInstance().Get<Transform>(id) };
					return r_transform.position;
				}

				return vec2{ 0.f, 0.f };
			}

			/*!***********************************************************************************
			\brief Returns the rotation of the transform of the entity passed in.

			\param[in] transformId - ID of the entity to retrieve the rotation of.
			*************************************************************************************/
			static float GetEntityOrientation(EntityID const id)
			{
				if (EntityManager::GetInstance().Has<Transform>(id))
				{
					Transform& r_transform{ EntityManager::GetInstance().Get<Transform>(id) };
					return r_transform.orientation;
				}

				return 0.f;
			}

			/*!***********************************************************************************
			 \brief Returns the scale of the transform of the entity passed in.

			 \param[in] transformId - ID of the entity to retrieve the scale of.
			*************************************************************************************/
			static vec2 GetEntityScale(EntityID const id)
			{
				if (EntityManager::GetInstance().Has<Transform>(id))
				{
					Transform& r_transform{ EntityManager::GetInstance().Get<Transform>(id) };
					return vec2{ r_transform.width, r_transform.height };
				}
				
				return vec2{ 0.f, 0.f };
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
				// Toggle the entity
				if (EntityManager::GetInstance().Has<EntityDescriptor>(id))
					EntityManager::GetInstance().Get<EntityDescriptor>(id).isActive = setToActive;
			}

			/*!***********************************************************************************
			 \brief Adjusts the position of the transform to the value passed in.

			 \param[in] idd - ID of the entity to update the transform of.
			 \param[in] r_position - Position to set the transform to.
			*************************************************************************************/
			static void PositionEntity(EntityID const id, vec2 const& r_position)
			{
				if (EntityManager::GetInstance().Has<Transform>(id))
				{
					Transform& r_transform{ EntityManager::GetInstance().Get<Transform>(id) }; // Get the transform of the player
					r_transform.position = r_position;
				}
			}

			/*!***********************************************************************************
			 \brief Adjusts the scale of the transform to the value passed in.

			 \param[in] id - ID of the entity to update the transform of.
			 \param[in] width - Width to set the transform to.
			 \param[in] height - Height to set the transform to.
			*************************************************************************************/
			static void ScaleEntity(EntityID const id, float const width, float const height)
			{
				if (EntityManager::GetInstance().Has<Transform>(id))
				{
					Transform& r_transform{ EntityManager::GetInstance().Get<Transform>(id) }; // Get the transform of the player
					r_transform.width = width;
					r_transform.height = height;
				}
			}

			/*!***********************************************************************************
			 \brief Sets the colour of the entity.

			 \param[in] id - ID of the entity to set the colour of
			 \param[in] color - color to set the entity to
			*************************************************************************************/
			static void SetColor(EntityID const id, vec4 const& color)
			{
				if(EntityManager::GetInstance().Has<Graphics::Renderer>(id))
					EntityManager::GetInstance().Get<Graphics::Renderer>(id).SetColor(color.x, color.y, color.z, color.w);
			}

			/*!***********************************************************************************
			\brief Identifies if the entity passed in is a cat. Assumes that cats have
						the keyword "Cat" somewhere in their name in entityDescriptor.

			\param[in] id - ID of the entity to check.
			*************************************************************************************/
			static bool IsActive(EntityID const id)
			{
				return (EntityManager::GetInstance().Has<EntityDescriptor>(id) && 
						EntityManager::GetInstance().Get<EntityDescriptor>(id).isActive);
			}

			/*!***********************************************************************************
			\brief Identifies if the entity passed in is a cat. Assumes that cats have
						the keyword "Cat" somewhere in their name in entityDescriptor.

			\param[in] id - ID of the entity to check.
			*************************************************************************************/
			static bool IsCat(EntityID const id)
			{
				return (EntityManager::GetInstance().Has<EntityDescriptor>(id) && 
						EntityManager::GetInstance().Get<EntityDescriptor>(id).name.find("Cat") != std::string::npos);
			}

			/*!***********************************************************************************
			 \brief Identifies if the entity passed in is an enemy. Assumes that enemies have
					the keyword "Rat" somewhere in their name in entityDescriptor.

			 \param[in] id - ID of the entity to check.
			*************************************************************************************/
			static bool IsEnemy(EntityID const id)
			{
				return (EntityManager::GetInstance().Has<EntityDescriptor>(id) && 
						EntityManager::GetInstance().Get<EntityDescriptor>(id).name.find("Rat") != std::string::npos);
			}

			/*!***********************************************************************************
			 \brief Identifies if the entity passed in is an obstacle. Assumes that obstacles
							have the keyword "Obstacle" somewhere in their name in entityDescriptor.

			 \param[in] id - ID of the entity to check.
			*************************************************************************************/
			static bool IsObstacle(EntityID const id)
			{
				return (EntityManager::GetInstance().Has<EntityDescriptor>(id) && 
						EntityManager::GetInstance().Get<EntityDescriptor>(id).name.find("Obstacle") != std::string::npos);
			}			
		};
	
}

