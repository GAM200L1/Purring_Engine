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
		/*!***********************************************************************************
		 \brief Construct a new Grid I D object
		 
		 \param[in] newX - value to set for variable x
		 \param[in] newY - value to set for variable y
		*************************************************************************************/
		GridID(int newX, int newY) : x{newX}, y{newY}{}
		int x, y;
	};
	
	class Cell
	{
	public:
		// ----- Constructor/Destructor ----- //
		/*!***********************************************************************************
		 \brief Construct a new Cell object
		 
		*************************************************************************************/
		Cell();
		
		/*!***********************************************************************************
		 \brief Construct a new Cell object
		 
		 \param[in] r_center - center of the cell created
		 \param[in] cellWidth - width of each cell in the grid
		*************************************************************************************/
		Cell(vec2 const& r_center, float cellWidth);
		
		/*!***********************************************************************************
		 \brief Destroy the Cell object, clears internal vector
		 
		*************************************************************************************/
		~Cell();

		// ----- Public Methods ----- //
		/*!***********************************************************************************
		 \brief Adds entity id to internal vector if it doesn't already exist
		 
		 \param[in] id - id to add
		*************************************************************************************/
		void Add(EntityID id);
		
		/*!***********************************************************************************
		 \brief Removes a specific id from internal vector if it exists
		 
		 \param[in] id - id to remove
		*************************************************************************************/
		void Remove(EntityID id);

		/*!***********************************************************************************
		 \brief Clears the internal vector in Cell
		 
		*************************************************************************************/
		void ClearCell();

		// ----- Public Checks ----- //
		/*!***********************************************************************************
		 \brief Checks if the id is an element in the internal vector
		 
		 \param[in] id - id to search for
		 \return true - id is in vector
		 \return false - id is not in vector
		*************************************************************************************/
		bool CheckForID(EntityID id);

		/*!***********************************************************************************
		 \brief Checks if there is at least 2 objects in the Cell to determine if collision
		 		needs to be tested
		 
		 \return true - there are more than 2 objects
		 \return false - there are less than 2 objects
		*************************************************************************************/
		bool CheckToTest() const;

		// ----- Getter ----- //
		/*!***********************************************************************************
		 \brief Obtain a const& to the internal vector
		 
		 \return std::vector<EntityID> const& - the internal vector in the cell
		*************************************************************************************/
		std::vector<EntityID> const& GetEntityIDs();

	private:
		// ----- Private Variables ----- //
		vec2 m_center; // center coordinate of the cell in world space
		vec2 m_min; // min bounds of cell
		vec2 m_max; // max bounds of cell
		std::vector<EntityID> m_entitiesInCell; // vector of ids that are in the cell
	};

	class Grid
	{
	public:
		// ----- Public Variables ----- //
		std::vector<std::vector<Cell>> m_cells; // a vector of vectors of cells - accessed with [col][row]

	public:
		// ----- Constructor/Destructor ----- //
		/*!***********************************************************************************
		 \brief Construct a new Grid object
		 
		*************************************************************************************/
		Grid();
		/*!***********************************************************************************
		 \brief Destroy the Grid object, clearing internal vector
		 
		*************************************************************************************/
		~Grid();
		
		// ----- Public Methods ----- //
		/*!***********************************************************************************
		 \brief Sets up the grid, used when scene is played. 
		 
		 \param[in] gridWidth - width of the grid
		 \param[in] gridHeight - height of the grid
		*************************************************************************************/
		void SetupGrid(float gridWidth, float gridHeight);

		/*!***********************************************************************************
		 \brief Updates the grid and the entities each cell contains. Called everytime 
		 		before testing for collision.
		 
		*************************************************************************************/
		void UpdateGrid();
		/*!***********************************************************************************
		 \brief Clears the entire grid of cells and columns.
		 
		*************************************************************************************/
		void ClearGrid();

		// ----- Public Checks ----- //
		/*!***********************************************************************************
		 \brief Checks if the grid has been set up
		 
		 \return true - grid has already been set up
		 \return false - grid has not been set up yet
		*************************************************************************************/
		bool GridExists() const;

		// ----- Public Methods ----- //
		/*!***********************************************************************************
		 \brief Checks if an AABB collider is in a grid area
		 
		 \param[in] r_collider const reference to an AABB collider
		 \return true - Collider is in grid area
		 \return false - Collider is not in grid area
		*************************************************************************************/
		bool CheckInGridArea(AABBCollider const& r_collider);
		
		/*!***********************************************************************************
		 \brief Checks if a Circle collider is in a grid area
		 
		 \param[in] r_collider const referenceto a Circle collider
		 \return true - Collider is in grid area
		 \return false - Collider is not in grid area
		*************************************************************************************/
		bool CheckInGridArea(CircleCollider const& r_collider);

		// ----- Public Methods ------ //
		/*!***********************************************************************************
		 \brief Get the GridID of the min and max bounds of an AABB collider

		 \param[in] r_collider - AABB Collider
		 \return std::pair<GridID, GridID> - GridID of the min and max bounds of the collider
		*************************************************************************************/
		std::pair<GridID, GridID> GetMinMaxIDs(AABBCollider const& r_collider);

		/*!***********************************************************************************
		 \brief Get the GridID of the min and max bounds of a Circle collider

		 \param[in] r_collider - Circle Collider
		 \return std::pair<GridID, GridID> - GridID of the min and max bounds of the collider
		*************************************************************************************/
		std::pair<GridID, GridID> GetMinMaxIDs(CircleCollider const& r_collider);

		/*!***********************************************************************************
		 \brief Get the GridID of a point
		 
		 \param[in] posX - x coordinate of the point
		 \param[in] posY - y coordinate of the point
		 \return GridID - ID of the grid the point is in
		*************************************************************************************/
		GridID GetIndex(float posX, float posY);
		
		// ----- Getters/Setters ----- //
		/*!***********************************************************************************
		 \brief Get the Grid Size object
		 
		 \return vec2 const& - m_gridSize, where x is width and y is height
		*************************************************************************************/
		vec2 const& GetGridSize() const;
		
		/*!***********************************************************************************
		 \brief Get the Grid Min object which is the coordinate of the min bound of the grid
		 
		 \return vec2 const& - m_min
		*************************************************************************************/
		vec2 const& GetGridMin() const { return m_min; }

		/*!***********************************************************************************
		 \brief Get the Grid Max object which is the coordinate of the max bound of the grid
		 
		 \return vec2 const& - m_max
		*************************************************************************************/
		vec2 const& GetGridMax() const { return m_max; }

		/*!***********************************************************************************
		 \brief Get the individual cell width
		 
		 \return float - m_cellWidth
		*************************************************************************************/
		float GetCellWitdh() const { return m_cellWidth; }

		/*!***********************************************************************************
		 \brief Get a vec2 with the number of columns and rows the grid has
		 
		 \return vec2 - number of columns and rows in x and y of vec2 respectively
		*************************************************************************************/
		vec2 GetColumnsRows() const { return vec2{ m_columns, m_rows }; }

	private:
	// ----- Private Variables ----- //
		int m_columns, m_rows; // contain the number of columns and rows the grid has
		vec2 m_min;
		vec2 m_max;
		vec2 m_gridSize;
		float m_cellWidth; // contain the half width of a cell, given a cell is a square
		bool m_gridHasSetup;
	};
}
