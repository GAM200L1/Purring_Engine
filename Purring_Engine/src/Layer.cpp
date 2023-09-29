/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     Layer.cpp
 \date     29-09-2023

 \author               Brandon HO Jun Jie
 \par      email:      brandonjunjie.ho@digipen.edu

 \brief	   This file contains the implementation a layer system that provides the
		   functionalities to attach and detach a layer. The layer is able to contain
		   and organise different components and systems of the engine.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/
#include "prpch.h"
#include "Layer.h"

PE::Layer::Layer(const std::string& debugName)
	:m_DebugName(debugName)
{

}

PE::Layer::~Layer()
{

}