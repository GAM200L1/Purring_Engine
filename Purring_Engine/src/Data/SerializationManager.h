#ifndef SERIALIZATION_MANAGER_H
#define SERIALIZATION_MANAGER_H

#include <unordered_map>
#include <utility>
#include <string>
#include "json.hpp"
#include <any>
#include <unordered_map>

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

#endif // SERIALIZATION_MANAGER_H
