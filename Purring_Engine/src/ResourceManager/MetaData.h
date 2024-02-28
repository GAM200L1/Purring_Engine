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
#include "Singleton.h"/*
#include "Graphics/ShaderProgram.h"
#include "Graphics/Texture.h"
#include "AudioManager/AudioManager.h"
#include "Graphics/Text.h"
#include "Animation/Animation.h"
#include "imgui.h"*/

namespace PE
{
    std::string GenerateDateTimeUID();

    void GenerateMetaFile(std::string const& r_filePath, std::string const& r_extension);

    class MetaData
    {
    public:
        MetaData() = default;
        MetaData(std::string extension) : Guid{ "guid" + extension + GenerateDateTimeUID()} {};
        nlohmann::json Serialize() const;
        MetaData& Deserialize(const nlohmann::json& r_j);

        std::string Guid{ "0" };
        std::string Type{ "MetaData" };
    };

    class TextureMetaData : public MetaData
    {
    public:
        nlohmann::json Serialize() const;
        TextureMetaData& Deserialize(const nlohmann::json& r_j);

        bool IsSpriteSheet{ false }; // false for single sprite
    };
}
