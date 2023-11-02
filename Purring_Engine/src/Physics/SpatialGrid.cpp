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

	Cell::Cell() :
		m_center{ 0.f, 0.f },
		m_min{ 0.f, 0.f },
		m_max{ 0.f, 0.f },
		m_entitiesInCell{} {}

	Cell::Cell(vec2 const& r_center, float cellWidth) : 
		m_center{ r_center }, 
		m_min{ r_center.x - (cellWidth * 0.5f), r_center.y - (cellWidth * 0.5f) },
		m_max{ r_center.x + (cellWidth * 0.5f), r_center.y + (cellWidth * 0.5f) }, 
		m_entitiesInCell {} {}

	Cell::~Cell()
	{
		ClearCell();
	}
	
	void Cell::Add(EntityID id)
	{
		// if id already exists in the cell don't need to add again
		if (CheckForID(id))
			return;

		m_entitiesInCell.emplace_back(id);
	}

	void Cell::Remove(EntityID id)
	{
		// there is nothing to remove
		if (m_entitiesInCell.empty())
			return;
		
		auto iter = std::remove(m_entitiesInCell.begin(), m_entitiesInCell.end(), id);
		iter;
	}

	bool Cell::CheckForID(EntityID id)
	{
		return (std::find(m_entitiesInCell.begin(), m_entitiesInCell.end(), id) != m_entitiesInCell.end());
	}

	bool Cell::CheckToTest() const
	{
		return (m_entitiesInCell.size() < 2);
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

	Grid::Grid() : 
		m_columns{ 0 }, m_rows{ 0 }, 
		m_min{ 0.f,0.f }, m_max{ 0.f, 0.f }, 
		m_cellWidth{ 0.f }, m_gridHasSetup{false} {}

	Grid::~Grid()
	{
		ClearGrid();
	}

	void Grid::SetupGrid(float gridWidth, float gridHeight)
	{
		m_gridHasSetup = true;

		auto checkForComponents = SceneView<Collider>();
		if (checkForComponents.begin() != checkForComponents.end())
		{
			int firstIndex = 0;
			for (EntityID colliderID : checkForComponents)
			{
				Transform const& r_transform = EntityManager::GetInstance().Get<Transform>(colliderID);
				
				// to fill up the width with a value first
				if (firstIndex == 0)
				{
					m_cellWidth = r_transform.height;
					++firstIndex;
				}

				m_cellWidth = (m_cellWidth < r_transform.height) ? m_cellWidth : r_transform.height;
				m_cellWidth = (m_cellWidth < r_transform.width) ? m_cellWidth : r_transform.width;
			}
		}
		else
		{
			// set arbitrary value for cellWidth
			m_cellWidth = std::min(gridWidth * 0.1f, gridHeight * 0.1f);
		}
		
		m_min = vec2{ 0.f - (gridWidth * 0.5f), 0.f - (gridHeight * 0.5f) };
		m_max = vec2{ 0.f + (gridWidth * 0.5f), 0.f + (gridHeight * 0.5f) };

		m_cellWidth *= 2.f;
		
		m_columns = (ceil(gridWidth / m_cellWidth));
		m_rows = (ceil(gridHeight / m_cellWidth));
		m_cells.resize(m_columns);
		// setup the vector that represents the grid. When accessing
		// an element it will be m_cells[col][row];
		for (size_t i{ 0 }; i < m_columns; ++i)
		{
			m_cells[i].resize(m_rows);
			vec2 bottomCellInColCenter{ m_min.x + (m_cellWidth * 0.5f * (i + 1)), m_min.y + (m_cellWidth * 0.5f) };
			for (size_t j{ 0 }; j < m_rows; ++j)
			{
				vec2 cellCenter{ bottomCellInColCenter.x, m_min.y + (m_cellWidth * 0.5f * (j+1)) };
				m_cells[i][j] = Cell{cellCenter, m_cellWidth};
			}
		}
	}


	void Grid::UpdateGrid()
	{
		for (auto& r_column : m_cells)
		{
			for (auto& r_cell : r_column)
			{
				r_cell.ClearCell();
			}
		}

		for (EntityID colliderID : SceneView<Collider, Transform>())
		{
			Collider const& r_collider = EntityManager::GetInstance().Get<Collider>(colliderID);
			std::pair<GridID, GridID> colliderMinMaxID{ {0,0}, {0,0} };
			std::visit([&](auto const& r_col)
			{
				colliderMinMaxID = GetMinMaxIDs(r_col);
			}, r_collider.colliderVariant);

			for (int col{ colliderMinMaxID.first.x }; col <= colliderMinMaxID.second.x; ++col)
			{
				for (int row{ colliderMinMaxID.first.y }; row <= colliderMinMaxID.second.y; ++row)
				{
					m_cells[col][row].Add(colliderID);
				}
			}
		}
	}

	void Grid::ClearGrid()
	{
		m_gridHasSetup = false;
		// clear each column of the row no. of cells it has
		for (auto& r_column : m_cells)
		{
			for (auto& r_cell : r_column)
			{
				r_cell.ClearCell();
			}
			r_column.clear();
		}
		m_cells.clear();
	}

	bool Grid::GridExists() const
	{
		return m_gridHasSetup;
	}

	std::pair<GridID, GridID> Grid::GetMinMaxIDs(AABBCollider const& r_collider)
	{
		GridID maxID = GetIndex(r_collider.max.x, r_collider.max.y);
		GridID minID = GetIndex(r_collider.min.x, r_collider.min.y);
		return std::pair<GridID, GridID>{ minID, maxID };
	}

	std::pair<GridID, GridID> Grid::GetMinMaxIDs(CircleCollider const& r_collider)
	{
		vec2 max{ r_collider.center.x + r_collider.radius, r_collider.center.y + r_collider.radius };
		vec2 min{ r_collider.center.x - r_collider.radius, r_collider.center.y - r_collider.radius };
		GridID maxID = GetIndex(max.x, max.y);
		GridID minID = GetIndex(min.x, min.y);
		return std::pair<GridID, GridID>{ minID, maxID };
	}

	GridID Grid::GetIndex(float posX, float posY)
	{
		vec2 lengths{ ((m_max - m_min) * 0.5f) };
		return GridID{ static_cast<int>((lengths.x + posX)/m_cellWidth), static_cast<int>((lengths.y + posY)/m_cellWidth) };
	}
}