/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     SceneView.cpp
 \date     13-09-2023
 
 \author               Foong Jun Wei
 \par      email:      f.junwei\@digipen.edu
 
 \brief     Empty by design.
 
 
 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/
#include "prpch.h"
/************************************************************************************************
*                                                                                               *
*                                       Empty file                                              *
*                                                                                               *
************************************************************************************************/

namespace PE
{
	int compare(const void* lhs, const void* rhs)
	{
		const size_t* x = (size_t*)lhs;
		const size_t* y = (size_t*)rhs;
		return (*x > *y) ? 1 : (*x < *y) ? -1 : 0;
	}
}
