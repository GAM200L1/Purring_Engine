/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     SpatialGrid.h
 \date     01-11-2023

 \author               Liew Yeni
 \par      email:      yeni.l/@digipen.edu.sg

 \brief 


 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/
#pragma once

#include "ECS/Entity.h"
#include "Colliders.h"

namespace PE
{
	using GridID = vec2;
	
	class Cell
	{
	public:
		inline void Add(EntityID id);
		inline void Remove(EntityID id);
		inline void ClearCell();
		inline std::vector<EntityID> const& GetEntityIDs();

	private:
		//vec2 m_center;
		//vec2 m_bounds; // min max
		//float m_halfWidth;
		std::vector<EntityID> m_entitiesInCell;
	};

	class Grid
	{
		Grid();
		~Grid();
		
		void SetupGrid(float gridWidth, float gridHeight);
		void UpdateGrid();
		void ClearGrid();

		inline std::unique_ptr<Cell>& GetCell(std::unique_ptr<Cell>& r_colliderCell, float posX, float posY, AABBCollider const& r_collider);
		inline std::unique_ptr<Cell>& GetCell(std::unique_ptr<Cell>& r_colliderCell, float posX, float posY, CircleCollider const& r_collider);
		//inline GridID GetIndex(float posX, float posY);


	private:
		size_t columns, rows; // contain the number of columns and rows the grid has
		vec2 gridSize;
		float cellWidth; // contain the half width of a cell, given a cell is a square
		std::vector<std::vector<std::unique_ptr<Cell>>> m_cells;
	};
}
