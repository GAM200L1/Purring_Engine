/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     Layer.h
 \date     29-09-2023

 \author               Brandon HO Jun Jie
 \par      email:      brandonjunjie.ho@digipen.edu

 \brief	   This file contains the implementation a layer system that provides the
		   functionalities to attach and detach a layer. The layer is able to contain
		   and organise different components and systems of the engine.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/
#pragma once
#include <string>


namespace PE
{
	class Layer
	{
	public:
		Layer(const std::string& name = "Layer");

		virtual ~Layer();

		virtual void OnAttach() {}
		virtual void OnDetach() {}

		virtual void OnUpdate() {}
		virtual void OnImGuiRender() {}
		virtual void OnEvent() {}

		const std::string& GetName() const { return m_DebugName; }

	protected:
		std::string m_DebugName;
	};

}