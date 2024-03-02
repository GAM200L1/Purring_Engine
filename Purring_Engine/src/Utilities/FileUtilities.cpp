/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     FileUtilities.cpp
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

 \brief    Contains definitions for helper functions for files


 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/

#include "prpch.h"
#include "FileUtilities.h"
#include "Data/SerializationManager.h"
#include "ResourceManager/MetaData.h"

namespace PE
{
	void GetFileNamesInParentPath(std::filesystem::path const& r_parentPath, std::vector<std::filesystem::path>& r_fileNames)
	{
		r_fileNames.clear();

		std::filesystem::path parentPath;

		// file path does not exist
		if (!std::filesystem::exists(r_parentPath))
		{
			parentPath = r_parentPath.parent_path();
		}
		else
		{
			parentPath = r_parentPath;
		}		

		for (std::filesystem::directory_entry const& r_dirEntry : std::filesystem::directory_iterator{ parentPath })
		{
			if (r_dirEntry.path().extension().string() != ".meta")
			{
				r_fileNames.emplace_back(parentPath.string() + "/" + r_dirEntry.path().filename().string());
			}
		}

		GenerateMetaFiles();
	}

	void GenerateMetaFiles()
	{
		std::vector<std::filesystem::path> resourceFiles;
		std::vector<std::filesystem::path> metaFiles;

		try
		{
			for (std::filesystem::directory_entry const& r_dirEntry : std::filesystem::recursive_directory_iterator{ "../Assets" })
			{
				std::filesystem::path filePath = r_dirEntry.path();
				filePath.make_preferred();

				if (filePath.has_extension())
				{
					if (filePath.extension().string() == ".meta")
					{
						metaFiles.emplace_back(filePath);
					}
					else
					{
						resourceFiles.emplace_back(filePath);
					}
				}
			}

			// loop through resource files and check if there is a corresponding meta file
			for (std::filesystem::path& filePath : resourceFiles)
			{
				auto metaFileIt{ std::find(metaFiles.begin(), metaFiles.end(), filePath.string() + ".meta") };

				// if there is a corresponding meta file, remove from metafile list
				if (metaFileIt != metaFiles.end())
				{
					metaFiles.erase(metaFileIt);
				}
				else // there is no corresponding meta file, generate one and remove file from resource list
				{
					// generate meta file
					std::cout << "Generate " + filePath.string() + ".meta" << std::endl;

					GenerateMetaFile(filePath.string() + ".meta", filePath.extension().string());
				}
			}

			// loop through orphaned meta files and delete them
			for (std::filesystem::path& filePath : metaFiles)
			{
				std::cout << filePath.string() << std::endl;
				std::filesystem::remove(filePath);
			}
		}
		catch (std::filesystem::filesystem_error const&)
		{
			// file directory is open
		}
	}

	void ClearMetaFiles()
	{
		for (std::filesystem::directory_entry const& r_dirEntry : std::filesystem::recursive_directory_iterator{ "../Assets" })
		{
			std::filesystem::path filePath = r_dirEntry.path();
			filePath.make_preferred();

			if (filePath.has_extension())
			{
				if (filePath.extension().string() == ".meta")
				{
					std::filesystem::remove(filePath);
				}
			}
		}
	}
}