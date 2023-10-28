/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     SerializationManager.h
 \date     25-09-2023

 \author               Hans (You Yang) ONG
 \par      email:      youyang.o@digipen.edu

 \brief	   This file contains the implementation of the SerializationManager class.
           The SerializationManager is responsible for serializing and deserializing entities
           to and from JSON format. It offers an interface for saving serialized entities to files
           and loading them back. The manager also handles the initialization of various component
           types during the deserialization process.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/
#pragma once

/*                                                                                                          includes
--------------------------------------------------------------------------------------------------------------------- */

#include "json.hpp"
//#include "ECS/Components.h"
//#include "ECS/Entity.h"
#include "Math/Transform.h"
#include "Physics/RigidBody.h"
#include "Physics/Colliders.h"
#include "Graphics/Renderer.h"
#include "Graphics/GUIRenderer.h"
#include "Data/JsonUtils.h"


struct StructPlayerStats
{
    int m_health;
    int m_level;
    float m_experience;
    std::string m_playerName;

    //void FromJson(const nlohmann::json& j, int& r_entityId, class SerializationManager& r_sm);
    //nlohmann::json ToJson(int r_entityId, class SerializationManager& r_sm) const;
};

struct StructEntity
{
    std::string m_name;                                   // Entity name
    std::unordered_map<std::string, std::any> m_data;     // Variable names and their data

    // Additional data members
    int m_id{ 0 };
    int m_someInt;
    float m_someFloat;
    double m_someDouble;
    char m_someChar;
    bool m_someBool;
    std::string m_someString;
};

class SerializationManager
{
    // ----- Public Methods ----- //
public:
    /*!***********************************************************************************
     \brief Opens a file explorer and returns the selected file path as a string.
    *************************************************************************************/
    std::string OpenFileExplorer();

    /*!***********************************************************************************
     \brief Serialize all entities in the scene into a single JSON object.
    *************************************************************************************/
    nlohmann::json SerializeAllEntities();

    /*!***********************************************************************************
     \brief Deserialize a JSON object to recreate all entities in the scene.
    *************************************************************************************/
    void DeserializeAllEntities(const nlohmann::json& r_j);

    /*!***********************************************************************************
     \brief Save the serialized JSON of all entities to a file with the given filename.
    *************************************************************************************/
    void SaveAllEntitiesToFile(const std::filesystem::path& filepath);

    /*!***********************************************************************************
     \brief Load all entities from a file with the given filename and deserialize them into the scene.
    *************************************************************************************/
    void LoadAllEntitiesFromFile(const std::filesystem::path& filepath);

    /*!***********************************************************************************
     \brief Serialize the entity with the given ID to a JSON object.
    *************************************************************************************/
    nlohmann::json SerializeEntity(int entityId);

    /*!***********************************************************************************
     \brief Deserialize a JSON object to create an entity, returning its ID.
    *************************************************************************************/
    size_t DeserializeEntity(const nlohmann::json& r_j);

    /*!***********************************************************************************
     \brief Save the serialized entity to a file.
    *************************************************************************************/
    void SaveToFile(const std::filesystem::path& filepath, int entityId);

    /*!***********************************************************************************
     \brief Load an entity from a serialized file, returning its ID.
    *************************************************************************************/
    size_t LoadFromFile(const std::filesystem::path& filepath);

    /*!************************************************************************
     \brief Serializes an entity's component to JSON.

     Serializes the ComponentType of the specified entity and adds it to the
     provided JSON object with the given key.

     \tparam ComponentType Type of component to serialize.
     \param entityId Entity ID.
     \param jsonKey Key for the serialized data.
     \param json JSON object for output.
    *************************************************************************/
    template<typename ComponentType>
    void SerializeComponent(int entityId, const std::string& jsonKey, nlohmann::json& json);


    // ----- Private Methods ----- //
private:
    /*!***********************************************************************************
     \brief Load the serialization functions for different components.
    *************************************************************************************/
    void LoadLoaders();

    /*!***********************************************************************************
     \brief Load the RigidBody component from JSON.
    *************************************************************************************/
    bool LoadRigidBody(const size_t& r_id, const nlohmann::json& r_json);

    /*!***********************************************************************************
     \brief Load the Collider component from JSON.
    *************************************************************************************/
    bool LoadCollider(const size_t& r_id, const nlohmann::json& r_json);

    /*!***********************************************************************************
     \brief Load the Transform component from JSON.
    *************************************************************************************/
    bool LoadTransform(const size_t& r_id, const nlohmann::json& r_json);

    /*!***********************************************************************************
     \brief Load the Renderer component from JSON.
    *************************************************************************************/
    bool LoadRenderer(const size_t& r_id, const nlohmann::json& r_json);

    bool LoadCamera(const size_t& r_id, const nlohmann::json& r_json);


    // ----- Private Methods ----- //
private:
    /*!***********************************************************************************
     \brief Map of entities identified by integer IDs.
    *************************************************************************************/
    std::unordered_map<int, StructEntity> m_entities;              // Store entities with integer IDs

    /*!***********************************************************************************
     \brief Default struct name used for serialization.
    *************************************************************************************/
    std::string m_structName = "Entity";                           // Default struct name for serialization

    /*!***********************************************************************************
     \brief Function pointer map for initializing components.
    *************************************************************************************/
    typedef bool(SerializationManager::* FnptrVoidptrLoad)(const size_t& r_id, const nlohmann::json& r_json);
    std::map<std::string, FnptrVoidptrLoad> m_initializeComponent;
};

/*!*****************************************************************************
 \brief Serializes an entity's component into a JSON object.

 Serializes the specified component of the given entity, adding it to the provided
 JSON object with the specified key.

 \tparam ComponentType Type of the component to serialize.
 \param entityId ID of the entity.
 \param jsonKey Key for the serialized component in the JSON object.
 \param json JSON object to store the serialized component.
*******************************************************************************/
template<typename ComponentType>
void SerializationManager::SerializeComponent(int entityId, const std::string& jsonKey, nlohmann::json& json)
{
    PE::EntityManager& entityManager = PE::EntityManager::GetInstance();
    if (entityManager.Has(static_cast<EntityID>(entityId), entityManager.GetComponentID<ComponentType>()))
    {
        ComponentType* component = static_cast<ComponentType*>(
            entityManager.GetComponentPoolPointer(entityManager.GetComponentID<ComponentType>())->Get(static_cast<EntityID>(entityId))
            );

        if (component != nullptr)
        {
            nlohmann::json jComponent = component->ToJson();
            json["Entity"]["components"][jsonKey] = jComponent;
        }
    }
}

