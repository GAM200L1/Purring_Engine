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

#include <vector>
#include <memory>
#include "ECS/Entity.h"
#include "Colliders.h"

namespace PE
{
	struct GridID
	{
		GridID(int newX, int newY) : x{newX}, y{newY}{}
		int x, y;
	};
	
	class Cell
	{
	public:
		// ----- Constructor/Destructor ----- //
		Cell(vec2 const& r_center, float cellWidth);
		~Cell();

		// ----- Public Methods ----- //
		inline void Add(EntityID id);
		inline void Remove(EntityID id);
		inline bool CheckForID(EntityID id);
		inline bool CheckToTest() const;
		inline void ClearCell();
		inline std::vector<EntityID> const& GetEntityIDs();

	private:
		// ----- Public Methods ----- //
		vec2 m_center;
		vec2 m_min;
		vec2 m_max;
		std::vector<EntityID> m_entitiesInCell;
	};

	class Grid
	{
	public:
		// ----- Public Variables ----- //
		std::vector<std::vector<std::unique_ptr<Cell>>> m_cells;

	public:
		// ----- Constructor/Destructor ----- //
		Grid();
		~Grid();
		
		// ----- Public Methods ----- //
		void SetupGrid(float gridWidth, float gridHeight);
		void UpdateGrid();
		void ClearGrid();

		// ----- Public Checks ----- //
		inline bool GridExists() const;

		// ----- Public Methods ----- //
		inline std::pair<GridID, GridID>& GetMinMaxIDs(AABBCollider const& r_collider);
		inline std::pair<GridID, GridID>& GetMinMaxIDs(CircleCollider const& r_collider);
		inline GridID GetIndex(float posX, float posY);

	private:
	// ----- Private Variables ----- //
		size_t m_columns, m_rows; // contain the number of columns and rows the grid has
		vec2 m_min;
		vec2 m_max;
		float m_cellWidth; // contain the half width of a cell, given a cell is a square
		bool m_gridHasSetup;
	};
}
