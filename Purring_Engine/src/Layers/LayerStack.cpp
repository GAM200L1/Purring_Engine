/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     LayerStack.cpp
 \date     29-09-2023

 \author               Brandon HO Jun Jie
 \par      email:      brandonjunjie.ho@digipen.edu

 \brief	   This file contains the implementation of the layer stack system that provides
		   the functionality to insert and remove layers in order.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/
#include "prpch.h"
#include "LayerStack.h"

namespace PE
{
	LayerStack::~LayerStack()
	{
		for (Layer* layer : m_Layers)
		{
			delete layer;
		}
	}

	void LayerStack::PushLayer(Layer* layer)
	{
		m_Layers.emplace(Begin() + m_layerInsertIndex, layer);
		++m_layerInsertIndex;
	}

	void LayerStack::PushOverlay(Layer* overlay)
	{
		m_Layers.emplace_back(overlay);
	}

	void LayerStack::PopLayer(Layer* layer)
	{
		std::vector<Layer*>::iterator it = std::find(Begin(), End(), layer);
		
		if (it != End())
		{
			m_Layers.erase(it);
			--m_layerInsertIndex;
		}
	}

	void LayerStack::PopOverlay(Layer* overlay)
	{
		std::vector<Layer*>::iterator it = std::find(Begin(), End(), overlay);

		if (it != End())
		{
			m_Layers.erase(it);
		}
	}
}