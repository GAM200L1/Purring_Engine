/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     FileUtilities.h
 \date     22-10-2023

 \author               Liew Yeni
 \par      email:      yeni.l/@digipen.edu

 \brief    Contains declarations for helper functions for files


 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/

#pragma once
#include <filesystem>
#include <string>
#include <vector>

namespace PE
{
	/*!***********************************************************************************
	 \brief Gets the names of the files in a specified path, usually a directory

	 \param[in] r_parentPath	path to retrieve names of its files from
	 \param[in] r_fileNames		vector to save retrieved names to
	*************************************************************************************/
	void GetFileNamesInParentPath(std::filesystem::path const& r_parentPath, std::vector<std::filesystem::path>& r_fileNames);

	void GenerateMetaFiles();

	void ClearMetaFiles();
}