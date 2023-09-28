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

class SerializationManager
{
public:
    nlohmann::json serializeEntity(int entityID);
    EntityID deserializeEntity(const nlohmann::json& j);

    void saveToFile(const std::string& filename, int entityID);
    EntityID loadFromFile(const std::string& filename);

private:
    std::unordered_map<int, Entity> entities; // Store entities with integer IDs
    std::string structName = "Entity"; // Default struct name for serialization
};
