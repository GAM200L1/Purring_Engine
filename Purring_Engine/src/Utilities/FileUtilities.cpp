/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     FileUtilities.cpp
 \date     22-10-2023

 \author               Liew Yeni
 \par      email:      yeni.l/@digipen.edu

 \brief    Contains definitions for helper functions for files


 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/

#include "prpch.h"
#include "FileUtilities.h"

namespace PE
{
	void GetFileNamesInParentPath(std::filesystem::path const& r_parentPath, std::vector<std::filesystem::path>& r_fileNames)
	{
		r_fileNames.clear();

		for (std::filesystem::directory_entry const& r_dirEntry : std::filesystem::directory_iterator{ r_parentPath })
		{
			r_fileNames.emplace_back(r_parentPath.string() + "/" + r_dirEntry.path().filename().string());
		}
	}
}