#pragma once
#include "prpch.h"
#include "json.hpp"
#include "ECS/Components.h"
#include "ECS/Entity.h"
#include "Math/Transform.h"
#include "Physics/RigidBody.h"
#include "Physics/Colliders.h"
#include "Graphics/Renderer.h"

struct PlayerStats
{
    int health;
    int level;
    float experience;
    std::string playerName;

    void from_json(const nlohmann::json& j, int& entityId, class SerializationManager& sm);
    nlohmann::json to_json(int entityId, class SerializationManager& sm) const;
};

// Structure to represent an Entity with variable names and data
struct Entity
{
    std::string name;                                   // Entity name
    std::unordered_map<std::string, std::any> data;     // Variable names and their data

    // Additional data members
    int id;
    int someInt;
    float someFloat;
    double someDouble;
    char someChar;
    bool someBool;
    std::string someString;
};

// Class for managing serialization and deserialization
class SerializationManager
{
public:
    // @JWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW
    // A map to store the mapping between entity IDs and their corresponding names
    std::unordered_map<int, std::string> entityIdToNameMap;

    // Method to set the name associated with an entity ID
    void setEntityName(int entityId, const std::string& name)
    {
        entityIdToNameMap[entityId] = name;
    }

    // Method to retrieve the name associated with an entity ID
    std::string getEntityName(int entityId)
    {
        if (entityIdToNameMap.find(entityId) != entityIdToNameMap.end())
        {
            return entityIdToNameMap[entityId];
        }
        return "";
    }




    // @JW I comment this out as I was test setEntityName and getEntityName functions above**
    //// New function to serialize a PlayerStats object.
    //nlohmann::json serializePlayerStats(const PlayerStats& stats) {
    //    return stats.to_json();
    //}

    //// New function to deserialize a PlayerStats object.
    //PlayerStats deserializePlayerStats(const nlohmann::json& j) {
    //    PlayerStats stats;
    //    stats.from_json(j);
    //    return stats;
    //}




    // Set an entity with a given ID
    void setEntity(int id, const Entity& entity)
    {
        entities[id] = entity;
    }

    // Get an entity by its ID
    Entity getEntity(int id)
    {
        return entities[id];
    }

    // Set the name of the struct for serialization
    void setStructName(const std::string& name)
    {
        structName = name;
    }

    // Serialize an entity into JSON format
    nlohmann::json serializeEntity(int entityID);

    // Deserialize JSON into an Entity and an ID
    std::pair<Entity, int> deserializeEntity(const nlohmann::json& j);

    // Save a serialized entity to a file
    void saveToFile(const std::string& filename, int entityID);

    // Load an entity from a file and return it with its ID
    std::pair<Entity, int> loadFromFile(const std::string& filename);

private:
    std::unordered_map<int, Entity> entities; // Store entities with integer IDs
    std::string structName = "Entity"; // Default struct name for serialization
};
