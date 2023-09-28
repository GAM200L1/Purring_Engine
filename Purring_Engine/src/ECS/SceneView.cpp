/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     SceneView.cpp
 \date     13-09-2023
 
 \author               Foong Jun Wei
 \par      email:      f.junwei\@digipen.edu
 
 \brief     Contains function defenition for compare function (follows the 
			requirements of std::qsort)
 
 
 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/
#include "prpch.h"

namespace PE
{
	int compare(const void* p_lhs, const void* p_rhs)
	{
		const size_t* x = (size_t*)p_lhs;
		const size_t* y = (size_t*)p_rhs;
		return (*x > *y) ? 1 : (*x < *y) ? -1 : 0;
	}
}
