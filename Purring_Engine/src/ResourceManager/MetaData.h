/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     MetaData.h
 \date     28-02-2024

 \author               Brandon HO Jun Jie
 \par      email:      brandonjunjie.ho@digipen.edu

 \brief	   This file contains the declarations of the Metadata clases. MetaData
            is the base class for all metadata, and TextureMetaData is a derived class.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/
/*                                                                                                          includes
--------------------------------------------------------------------------------------------------------------------- */

namespace PE
{
    /*!***********************************************************************************
     \brief Generates a unique identifier based on the current date and time.
    *************************************************************************************/
    std::string GenerateDateTimeUID();

    /*!***********************************************************************************
     \brief Generates a meta file for a file with the specified extension.

     \param[in] r_filePath		path to the file to generate a meta file for
     \param[in] r_extension		extension of the file
    *************************************************************************************/
    void GenerateMetaFile(std::string const& r_filePath, std::string const& r_extension);

    class MetaData
    {
    public:
        MetaData() = default;

        /*!***********************************************************************************
         \brief Constructor for the MetaData class.

         \param[in] extension		extension of the file
        *************************************************************************************/
        MetaData(std::string extension) : Guid{ "guid" + extension + GenerateDateTimeUID()} {};

        /*!***********************************************************************************
         \brief Serializes the metadata to a json object.
                 
         \return nlohmann::json		json object containing the serialized metadata
        *************************************************************************************/
        nlohmann::json Serialize() const;

        /*!***********************************************************************************
         \brief Deserializes the metadata from a json object.
                 
         \param[in] r_j		json object to deserialize from
                         				 
         \return MetaData&		reference to the deserialized metadata
        *************************************************************************************/
        MetaData& Deserialize(const nlohmann::json& r_j);

        std::string Guid{ "0" };
        std::string Type{ "MetaData" };
    };

    class TextureMetaData : public MetaData
    {
    public:
        /*!***********************************************************************************
         \brief Serializes the metadata to a json object.

         \return nlohmann::json		json object containing the serialized metadata
        *************************************************************************************/
        nlohmann::json Serialize() const;

        /*!***********************************************************************************
         \brief Deserializes the metadata from a json object.

         \param[in] r_j		json object to deserialize from

         \return MetaData&		reference to the deserialized metadata
        *************************************************************************************/
        TextureMetaData& Deserialize(const nlohmann::json& r_j);

        bool IsSpriteSheet{ false }; // false for single sprite
    };
}
