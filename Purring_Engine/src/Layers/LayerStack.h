/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     LayerStack.h
 \date     29-09-2023

 \author               Brandon HO Jun Jie
 \par      email:      brandonjunjie.ho@digipen.edu

 \brief	   This file contains the implementation of the layer stack system that provides
		   the functionality to insert and remove layers in order.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/
#pragma once

#include "Layer.h"
#include <vector>

namespace PE
{
	class LayerStack
	{
	public:
		LayerStack() = default;
		~LayerStack();

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);
		void PopLayer(Layer* layer);
		void PopOverlay(Layer* overlay);
		
		std::vector<Layer*>::iterator Begin() { return m_Layers.begin(); }
		std::vector<Layer*>::iterator End() { return m_Layers.end(); }

	private:
		std::vector<Layer*> m_Layers;

		// keeps track of where to insert layer as overlay
		// will be emplace from the back
		unsigned int m_layerInsertIndex = 0;
	};
}