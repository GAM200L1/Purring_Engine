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