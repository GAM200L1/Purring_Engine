/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     FileUtilities.h
 \date     22-10-2023

 \author               Liew Yeni
 \par      email:      yeni.l\@digipen.edu
 \par      code %:     35%
 \par      changes:    Original Author

 \co-author            Brandon Ho Jun Jie
 \par      email:      brandonjunjie.ho\@digipen.edu
 \par      code %:     65%
 \par      changes:    28-02-2024
					   Generating and clearing meta files

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

	/*!***********************************************************************************
	 \brief	Generates meta files for all files in the project directory.
	*************************************************************************************/
	void GenerateMetaFiles();

	/*!***********************************************************************************
	 \brief Clears all meta files in the project directory. Warning: This is irreversible.
	*************************************************************************************/
	void ClearMetaFiles();
}