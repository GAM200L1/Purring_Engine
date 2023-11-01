/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     RigidBody.cpp
 \date     10-09-2023

 \author               Liew Yeni
 \par      email:      yeni.l/@digipen.edu.sg

 \brief This function contains the declarations for the RigidBody class and EnumRigidBodyType enum class.
		Changes: Added in Serialization and Deserialization functions for RigidBody Component


 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/
#pragma once

#include "ECS/Entity.h"
#include "Colliders.h"

namespace PE
{
	using GridID = vec2;
	
	struct Cell
	{
		vec2 center;
		vec2 bounds; // min max
		float halfWidth;

		void Add(std::unique_ptr<RigidBody> p_body);
		void Remove(std::unique_ptr<RigidBody> p_body);

		std::vector<EntityID> entitiesInCell;
	};

	class Grid
	{
		Grid(float GridWidth, float GridHeight);
		~Grid();
		
		void CreateGrid();
		void UpdateGrid();

		void Clear();

		inline Cell GetCell(float posX, float posY);
		inline GridID GetIndex(float posX, float posY);


	private:
		float columns, rows; // contain the number of columns and rows the grid has
		vec2 gridSize{ 0.f, 0.f };
		vec2 cellSize; // contain the height and width each cell wil be
		std::vector<std::vector<std::unique_ptr<Cell>>> m_cells;
	};
}
