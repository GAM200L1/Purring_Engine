/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     SpatialGrid.cpp
 \date     01-11-2023

 \author               Liew Yeni
 \par      email:      yeni.l/@digipen.edu.sg

 \brief 


 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/

#include "prpch.h"
#include "ECS/SceneView.h"
#include "SpatialGrid.h"

namespace PE
{
	// ----- Cell Class Member Function Definitons ----- //

	void Cell::Add(EntityID id)
	{
		m_entitiesInCell.emplace_back(id);
	}

	void Cell::Remove(EntityID id)
	{
		for (auto iter = m_entitiesInCell.begin(); iter != m_entitiesInCell.end(); ++iter)
		{
			if (*iter == id)
			{
				m_entitiesInCell.erase(iter);
				return;
			}
		}
		// some debug message
	}

	void Cell::ClearCell()
	{
		m_entitiesInCell.clear();
	}

	std::vector<EntityID> const& Cell::GetEntityIDs()
	{
		return m_entitiesInCell;
	}

	// ----- Grid Class Member Function Declarations ----- //

	Grid::Grid() : columns{ 0 }, rows{ 0 }, gridSize{ 0.f, 0.f }, cellWidth{ 0.f } {}

	Grid::~Grid()
	{
		ClearGrid();
	}

	void Grid::SetupGrid(float gridWidth, float gridHeight)
	{
		for (EntityID colliderID : SceneView<Collider, Transform>())
		{
			Transform const& r_transform = EntityManager::GetInstance().Get<Transform>(colliderID);
			cellWidth = (cellWidth < r_transform.height) ? cellWidth : r_transform.height;
			cellWidth = (cellWidth < r_transform.width) ? cellWidth : r_transform.width;
		}
		
		cellWidth *= 2.f;
		gridSize = vec2{ gridWidth, gridHeight };
		
		columns = ceil(gridWidth / cellWidth);
		rows = ceil(gridHeight / cellWidth);


		// setup the vector that represents the grid. When accessing
		// an element it will be m_cells[col][row];
		std::vector<std::unique_ptr<Cell>> temp;
		temp.reserve(rows);

		for (size_t i{ 0 }; i < columns; ++i)
		{
			m_cells.emplace_back(temp);
		}
	}


	void Grid::UpdateGrid()
	{
		for (EntityID colliderID : SceneView<Collider, Transform>())
		{
			Collider const& r_collider = EntityManager::GetInstance().Get<Collider>(colliderID);
			vec2 const& r_position = EntityManager::GetInstance().Get<Transform>(colliderID).position;
			std::unique_ptr<Cell> p_colliderCell;
			std::visit([&](auto const& r_col)
			{
				GetCell(p_colliderCell, r_position.x, r_position.y, r_col);
			}, r_collider.colliderVariant);

			p_colliderCell->Add(colliderID);
		}
	}

	void Grid::ClearGrid()
	{
		// clear each column of the row no. of cells it has
		for (auto& r_column : m_cells)
		{
			r_column.clear();
		}
		m_cells.clear();
	}

	std::unique_ptr<Cell>& Grid::GetCell(std::unique_ptr<Cell>& r_colliderCell, float posX, float posY, AABBCollider const& r_collider)
	{
		*r_colliderCell = *m_cells[1][1];
	}

	std::unique_ptr<Cell>& Grid::GetCell(std::unique_ptr<Cell>& r_colliderCell, float posX, float posY, CircleCollider const& r_collider)
	{

	}
}