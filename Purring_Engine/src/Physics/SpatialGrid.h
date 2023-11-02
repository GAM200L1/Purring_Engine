/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     SpatialGrid.h
 \date     01-11-2023

 \author               Liew Yeni
 \par      email:      yeni.l/@digipen.edu.sg

 \brief	   This file contains function declarations for classes Cell and Grid
		   and defines the struct GridID.


 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/
#pragma once

#include <vector>
#include <memory>
#include "ECS/Entity.h"
#include "Colliders.h"

namespace PE
{
	//! struct that is used to hold the column and row that a collider would be on
	struct GridID
	{
		GridID(int newX, int newY) : x{newX}, y{newY}{}
		int x, y;
	};
	
	class Cell
	{
	public:
		// ----- Constructor/Destructor ----- //
		Cell();
		Cell(vec2 const& r_center, float cellWidth);
		~Cell();

		// ----- Public Methods ----- //
		void Add(EntityID id);
		void Remove(EntityID id);
		bool CheckForID(EntityID id);
		bool CheckToTest() const;
		void ClearCell();
		std::vector<EntityID> const& GetEntityIDs();

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
		std::vector<std::vector<Cell>> m_cells;

	public:
		// ----- Constructor/Destructor ----- //
		Grid();
		~Grid();
		
		// ----- Public Methods ----- //
		void SetupGrid(float gridWidth, float gridHeight);
		void UpdateGrid();
		void ClearGrid();

		// ----- Public Checks ----- //
		bool GridExists() const;

		// ----- Public Methods ----- //
		bool CheckInGridArea(AABBCollider const& r_collider);
		bool CheckInGridArea(CircleCollider const& r_collider);
		std::pair<GridID, GridID> GetMinMaxIDs(AABBCollider const& r_collider);
		std::pair<GridID, GridID> GetMinMaxIDs(CircleCollider const& r_collider);
		GridID GetIndex(float posX, float posY);

	private:
	// ----- Private Variables ----- //
		int m_columns, m_rows; // contain the number of columns and rows the grid has
		vec2 m_min;
		vec2 m_max;
		float m_cellWidth; // contain the half width of a cell, given a cell is a square
		bool m_gridHasSetup;
	};
}
