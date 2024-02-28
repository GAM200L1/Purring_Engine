/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     Animation.cpp
 \date     27-11-2023

 \author               Brandon HO Jun Jie
 \par      email:      brandonjunjie.ho@digipen.edu

 \brief	   This file contains the implementation of the Animation and AnimationManager
           classes. Animation handles the logic for updating and maintaining individual
           animation frames, while AnimationManager manages multiple Animation objects,
           offering an interface for creating, updating, and adding frames to animations.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/
/*                                                                                                          includes
--------------------------------------------------------------------------------------------------------------------- */
#include "prpch.h"
#include "ResourceManager.h"
#include "Data/SerializationManager.h"
#include "Logging/Logger.h"

extern Logger engine_logger;

namespace PE
{
    nlohmann::json MetaData::Serialize() const
    {
        nlohmann::json j;
        j["Guid"] = Guid;

        return j;
    }

    MetaData& MetaData::Deserialize(const nlohmann::json& r_j)
    {
        if (r_j.contains("Guid"))
            Guid = r_j["Guid"].get<bool>();

        return *this;
    }

    nlohmann::json TextureMetaData::Serialize() const
    {
        nlohmann::json j{ MetaData::Serialize() };
        j["IsSpriteSheet"] = IsSpriteSheet;

        return j;
    }

    TextureMetaData& TextureMetaData::Deserialize(const nlohmann::json& r_j)
    {
        MetaData::Deserialize(r_j);

        if (r_j.contains("IsSpriteSheet"))
        IsSpriteSheet = r_j["IsSpriteSheet"].get<bool>();

        return *this;
    }

    std::string GenerateDateTimeUID()
    {
        auto now = std::chrono::system_clock::now();
        auto now_c = std::chrono::system_clock::to_time_t(now);

        // Optionally, include milliseconds for more uniqueness
        auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()) % 1000;

        std::stringstream dateTimeUID;
        std::tm buf;
        ::localtime_s(&buf, &now_c);

        dateTimeUID << std::put_time(&buf, "%Y%m%d_%H%M%S")
            << '_' << std::setfill('0') << std::setw(3) << milliseconds.count();

        return dateTimeUID.str();
    }

    void GenerateMetaFile(std::string const& r_filePath, std::string const& r_extension)
    {
        if (r_extension == ".png")
        {
            TextureMetaData metaData{ r_extension };
            SerializationManager serializationManager;
            serializationManager.SaveMetaDataToFile(r_filePath, metaData.Serialize());
        }
        else
        {
            MetaData metaData{ r_extension };
            SerializationManager serializationManager;
            serializationManager.SaveMetaDataToFile(r_filePath, metaData.Serialize());
        }
    }
}
