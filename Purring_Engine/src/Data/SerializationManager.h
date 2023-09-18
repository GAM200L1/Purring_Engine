#ifndef SERIALIZATION_MANAGER_H
#define SERIALIZATION_MANAGER_H

#include <unordered_map>
#include <utility>
#include <string>
#include "json.hpp"
#include <any>
#include <unordered_map>

struct Entity
{
    std::string name; // struct name
    std::unordered_map<std::string, std::any> data; // variable names and their data

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
    void setEntity(int id, const Entity& entity)
    {
        entities[id] = entity;
    }

    Entity getEntity(int id)
    {
        return entities[id];
    }

    void setStructName(const std::string& name) {
        structName = name;
    }

    nlohmann::json serializeEntity(int entityID);
    std::pair<Entity, int> deserializeEntity(const nlohmann::json& j);
    void saveToFile(const std::string& filename, int entityID);
    std::pair<Entity, int> loadFromFile(const std::string& filename);

private:
    std::unordered_map<int, Entity> entities;
    std::string structName = "Entity";
};

#endif // SERIALIZATION_MANAGER_H
