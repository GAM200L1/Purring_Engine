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
		bool CheckInGridArea(AABBCollider const& r_collider);
		bool CheckInGridArea(CircleCollider const& r_collider);
		std::pair<GridID, GridID> GetMinMaxIDs(AABBCollider const& r_collider);
		std::pair<GridID, GridID> GetMinMaxIDs(CircleCollider const& r_collider);
		GridID GetIndex(float posX, float posY);
		vec2 const& GetGridSize() const;
		vec2 const& GetGridMin() const;
		vec2 const& GetGridMax() const;
		float GetCellWitdh() const;
		vec2 GetColumnsRows() const;

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
